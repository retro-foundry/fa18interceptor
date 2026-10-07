"""Compare controlled postflight conditions through the shared native runtime.

Source terminal flags are seeded only by the validation executable, after normal
mission startup. C1643A remains the explicit existing shared config-write
boundary; its disk/status decisions are outside this result-caller comparison.
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_postflight_schedule_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/postflight-schedule/original-check')
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    oracles = {}
    # These builds share GNU reference objects and must stay sequential.
    for name in ('mode_entry', 'frame_body'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True,
                       stdout=subprocess.DEVNULL)
        oracles[name] = oracle
    totals = {'entries': 0, 'bodies': 0, 'shared_config_writes': 0}
    for kind in ('four', 'five', 'ready'):
        case = work / kind
        case.mkdir(parents=True, exist_ok=True)
        prefix = case / 'frame'
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
                        if kind == 'ready':
                            values.append('--shared-config-write')
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
                        raise RuntimeError(comparison.stderr or comparison.stdout)
                    for line in comparison.stdout.splitlines():
                        if line.startswith('Shared native config-write boundary calls:'):
                            totals['shared_config_writes'] += int(line.split(':', 1)[1].split()[0])
        totals['entries'] += len(entries)
        totals['bodies'] += len(bodies)
        print(f'{kind}: {len(entries)} input/stage intervals and {len(bodies)} bodies match compared original RAM/display', flush=True)
    assert totals['shared_config_writes'] == 1, totals
    (work / 'comparison.json').write_text(json.dumps(totals, indent=2) + '\n')
    print('Controlled postflight paths pass; result caller uses the shared save boundary (C1643A disk/status decisions excluded)')


if __name__ == '__main__':
    main()
