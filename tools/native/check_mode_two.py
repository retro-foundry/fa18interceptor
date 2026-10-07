"""Check source modes 2, 3, 4, 5, 6, 7, 8 or 125 through native menu and flight.

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
    parser.add_argument('--mode', type=int, choices=(2,3,4,5,6,7,8,125), default=2)
    parser.add_argument('--aircraft', type=int, choices=(1,2), default=1)
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    prefix = work / 'frame'
    result = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                             str(work / 'pilot-test'), str(prefix), str(args.mode), str(args.aircraft)], cwd=ROOT,
                            capture_output=True, text=True, timeout=25)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    exports = [json.loads(line) for line in result.stdout.splitlines()]
    entries = [item for item in exports if 'entry' in item]
    bodies = [item for item in exports if 'capture' in item]
    assert len(entries) >= {2:32,3:43,4:39,5:39,6:38,7:42,8:38,125:55}[args.mode] and len(bodies) >= {2:21,3:29,4:27,5:27,6:27,7:29,8:27,125:35}[args.mode], exports
    required={'C10DAE'}
    if args.mode==2:
        required|={'C10272','C1029E','C102D8','C10302','C10362','C0F920'}
    elif args.mode==125:
        required|={'C10272','C1029E','C10C08','C0F992','C0FCB4'}
    else:
        required|={'C103E4','C10418','C10458','C104C2','C105A6','C105F4',
                   'C10626','C1064C','C10900','C10942','C10970','C109AC',
                   'C10A24','C10B1E'}
        if args.mode==3:
            required|={'C10AB2','C10AE6'}
        if args.mode==7:
            required|={'C11078','C110A4'}
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
    outcome={2:'returns to menu',3:f'runs over 768 scene frames with aircraft {args.aircraft}',4:'runs over 2000 scene frames',5:'runs over 2000 scene frames',6:'runs over 384 scene frames',7:'runs over 2000 scene frames with an unlocked saved pilot',8:'runs over 2000 scene frames with an unlocked saved pilot',
             125:'runs over 2,000 scene frames, including Escape/restart'}[args.mode]
    print(f'Mode {args.mode} {outcome}; {len(entries)} actual input/stage intervals and '
          f'{len(bodies)} frame bodies match compared original RAM/display')


if __name__ == '__main__':
    main()
