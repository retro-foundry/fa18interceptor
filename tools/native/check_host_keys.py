"""Compare live SDL rudder/keypad input and its native flight bodies to source."""
import argparse
import json
from pathlib import Path
import subprocess
import sys
from capture_workspace import CaptureWorkspace, retain_failure

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_host_keys_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/host-keys-check')
    args = parser.parse_args()
    work = args.out.resolve()
    with CaptureWorkspace(work) as captures:
        prefix = captures / 'frame'
        run = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
            str(work / 'pilot'), str(prefix)], cwd=ROOT, capture_output=True, text=True, timeout=30)
        (work / 'native-run.log').write_text(run.stdout + run.stderr)
        if run.returncode:
            raise RuntimeError(run.stderr or run.stdout)
        exports = [json.loads(line) for line in run.stdout.splitlines()]
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        assert len(entries) == 26 and len(bodies) == 52, exports
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        for name, cases in (('mode_entry', entries), ('frame_body', bodies)):
            oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
            build = subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                str(oracle.relative_to(ROOT)), '--main', f'tools/native/native_{name}_oracle.c'],
                cwd=ROOT, capture_output=True, text=True)
            (work / f'{name}-build.log').write_text(build.stdout + build.stderr)
            if build.returncode:
                raise RuntimeError(build.stderr or build.stdout)
            with (work / f'{name}-check.log').open('w') as log:
                for item in cases:
                    if name == 'mode_entry':
                        capture = str(prefix) + f".entry.{item['entry']}"
                        values = [str(item['tick']), *map(str, item['keys'])]
                    else:
                        capture = str(prefix) + f".{item['capture']}"
                        values = [str(item['before_tick']), str(item['after_tick']),
                            str(item['saved_tick']), capture + '.source.dat']
                    check = subprocess.run([str(oracle), capture + '.before.dat',
                        capture + '.after.dat', *values], cwd=ROOT, capture_output=True,
                        text=True, timeout=15)
                    log.write(check.stdout + check.stderr)
                    log.flush()
                    if check.returncode:
                        retain_failure(capture, work)
                        raise RuntimeError(check.stderr or check.stdout)
            print(f'{name}: {len(cases)} actual SDL-key runtime captures match original compared RAM/display', flush=True)


if __name__ == '__main__':
    main()
