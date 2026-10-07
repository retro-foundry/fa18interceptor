"""Check digit 3/mode 2 or digit 4/mode 125 through native menu and flight.

Starts the shared playable runtime from disk/input and compares original
C0F3C4/C0F5F8 intervals plus C0EFEA/C0F3C0 bodies. No full Amiga replay.
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mode_two_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mode-check')
    parser.add_argument('--mode', type=int, choices=(2,125), default=2)
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    prefix = work / 'frame'
    result = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                             str(work / 'pilot-test'), str(prefix), str(args.mode)], cwd=ROOT,
                            capture_output=True, text=True, timeout=25)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    exports = [json.loads(line) for line in result.stdout.splitlines()]
    entries = [item for item in exports if 'entry' in item]
    bodies = [item for item in exports if 'capture' in item]
    assert len(entries) >= (32 if args.mode==2 else 55) and len(bodies) >= (21 if args.mode==2 else 35), exports
    required={'C10272','C1029E','C10DAE'}
    if args.mode==2:
        required|={'C102D8','C10302','C10362','C0F920'}
    else:
        required|={'C10C08','C0F992','C0FCB4'}
    assert required <= {
        item['stage'] for item in bodies}, bodies
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    for name, captures in (('mode_entry', entries), ('frame_body', bodies)):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        # Shared GNU reference objects require sequential builds.
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        with (work / f'{name}-check.log').open('w') as log:
            for item in captures:
                if name == 'mode_entry':
                    capture = str(prefix) + f".entry.{item['entry']}"
                    values = [str(item['tick']), *map(str, item['keys'])]
                else:
                    capture = str(prefix) + f".{item['capture']}"
                    values = [str(item['before_tick']), str(item['after_tick']),
                              str(item['saved_tick']), capture + '.source.dat']
                    if item['owner_exit']:
                        values.append('owner-exit')
                comparison = subprocess.run([str(oracle), capture + '.before.dat',
                                             capture + '.after.dat', *values], cwd=ROOT,
                                            capture_output=True, text=True, timeout=15)
                log.write(comparison.stdout + comparison.stderr)
                log.flush()
                print(comparison.stdout, end='', flush=True)
                if comparison.returncode:
                    raise RuntimeError(comparison.stderr or comparison.stdout)
    outcome='returns to menu' if args.mode==2 else 'runs over 2,000 scene frames, including Escape/restart'
    print(f'Mode {args.mode} {outcome}; {len(entries)} actual input/stage intervals and '
          f'{len(bodies)} frame bodies match compared original RAM/display')


if __name__ == '__main__':
    main()
