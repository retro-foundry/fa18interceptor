"""Compare controlled postflight conditions through the shared native runtime.

Source terminal flags are seeded only by the validation executable, after normal
mission startup. Complete C1643A/C0EF08 source owners execute in the reference;
only their actual OS services use the shared writable-overlay host contract.
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys
from capture_workspace import CaptureWorkspace, retain_failure

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_postflight_schedule_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/postflight-schedule/original-check')
    parser.add_argument('--keep-captures', action='store_true', help='Retain all raw RAM for deliberate debugging')
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    with CaptureWorkspace(work, args.keep_captures) as capture_dir:
        check(args, work, capture_dir)


def check(args, work, capture_dir):
    oracles = {}
    # These builds share GNU reference objects and must stay sequential.
    for name in ('mode_entry', 'frame_body'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True,
                       stdout=subprocess.DEVNULL)
        oracles[name] = oracle
    totals = {'entries': 0, 'bodies': 0, 'source_dos_writes': 0}
    for kind in ('four', 'five', 'ready'):
        case = work / kind
        case.mkdir(parents=True, exist_ok=True)
        capture_case = capture_dir / kind
        capture_case.mkdir(parents=True, exist_ok=True)
        prefix = capture_case / 'frame'
        run = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                              str(case / 'pilot'), str(prefix), kind], cwd=ROOT,
                             capture_output=True, text=True, timeout=45)
        (case / 'run.log').write_text(run.stdout + run.stderr)
        if run.returncode:
            raise RuntimeError(run.stderr or run.stdout)
        exports = [json.loads(line) for line in run.stdout.splitlines()]
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        assert len(bodies) >= 8 and entries, exports
        if kind != 'five':
            assert any(item['stage'] == 'C110A4' for item in entries), exports
        (case / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        for name, captures in (('mode_entry', entries), ('frame_body', bodies)):
            with (case / f'{name}-check.log').open('w') as log:
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
                    comparison = subprocess.run([str(oracles[name]), capture + '.before.dat',
                                                 capture + '.after.dat', *values], cwd=ROOT,
                                                capture_output=True, text=True, timeout=15)
                    log.write(comparison.stdout + comparison.stderr)
                    log.flush()
                    if comparison.returncode:
                        retain_failure(capture, case)
                        raise RuntimeError(comparison.stderr or comparison.stdout)
                    for line in comparison.stdout.splitlines():
                        if line.startswith('Complete original file owners reached DOS Write'):
                            totals['source_dos_writes'] += int(line.split()[-2])
        totals['entries'] += len(entries)
        totals['bodies'] += len(bodies)
        print(f'{kind}: {len(entries)} input/stage intervals and {len(bodies)} bodies match compared original RAM/display', flush=True)
    assert totals['source_dos_writes'] == 1, totals
    (work / 'comparison.json').write_text(json.dumps(totals, indent=2) + '\n')
    print('Controlled postflight paths pass, including complete C1643A/C0EF08 source decisions and real overlay persistence')


if __name__ == '__main__':
    main()
