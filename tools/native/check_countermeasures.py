"""Verify keyboard/recorder countermeasures and original gameplay contracts.

The integration entry shares the playable runner's runtime objects and starts
from disk/input. Controlled input/collision parents follow ordinary Free Flight.
No original full replay is run; complete body comparisons retain HUD pixels.
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
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_countermeasures_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/countermeasure-check')
    parser.add_argument('--keep-captures', action='store_true', help='Retain all raw RAM for deliberate debugging')
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    with CaptureWorkspace(work, args.keep_captures) as capture_dir:
        check(args, work, capture_dir)


def check(args, work, capture_dir):
    prefix = capture_dir / 'frame'
    run = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                          str(work / 'pilot'), str(prefix)], cwd=ROOT,
                         capture_output=True, text=True, timeout=25)
    (work / 'native-run.log').write_text(run.stdout + run.stderr)
    if run.returncode:
        raise RuntimeError(run.stderr or run.stdout)
    exports = [json.loads(line) for line in run.stdout.splitlines()]
    bodies = [entry for entry in exports if 'capture' in entry]
    parents = [entry for entry in exports if 'control_parent' in entry]
    fd_inputs = [entry for entry in exports if 'fd_input' in entry]
    pending_inputs = [entry for entry in exports if 'pending_input' in entry]
    assert [body['capture'] for body in bodies] == list(range(4)), bodies
    assert [entry['control_parent'] for entry in parents] == list(range(4)), parents
    assert any(entry['collision_hit'] for entry in parents), parents
    assert [entry['fd_input'] for entry in fd_inputs] == [0, 1], fd_inputs
    assert [entry['pending_input'] for entry in pending_inputs] == list(range(24)), pending_inputs
    assert sum(entry['chain'] for entry in pending_inputs) == 12, pending_inputs
    assert {entry['recorder_mode'] for entry in pending_inputs} == {1, 2, 3}, pending_inputs
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    for name in ('input', 'control_effects', 'frame_body'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        # These reference builds share an object directory: keep sequential.
        build = subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT,
                        capture_output=True, text=True)
        (work / f'{name}-build.log').write_text(build.stdout + build.stderr)
        if build.returncode:
            raise RuntimeError(build.stderr or build.stdout)
        with (work / f'{name}-check.log').open('w') as log:
            def compare(capture, *values):
                comparison = subprocess.run([str(oracle), *map(str, values)],
                    cwd=ROOT, capture_output=True, text=True, timeout=15)
                log.write(comparison.stdout + comparison.stderr)
                log.flush()
                if comparison.returncode:
                    retain_failure(capture, work)
                    raise RuntimeError(comparison.stderr or comparison.stdout)
            if name != 'frame_body':
                compare(str(prefix) + '.0', str(prefix) + '.0.before.dat')
            if name == 'input':
                for entry in fd_inputs:
                    parent = str(prefix) + f".fd.{entry['fd_input']}"
                    compare(parent, parent + '.before.dat', parent + '.after.dat', entry['raw'])
                for entry in pending_inputs:
                    parent = str(prefix) + f".pending.{entry['pending_input']}"
                    compare(parent, parent + '.before.dat', parent + '.after.dat', 'pending')
            elif name == 'control_effects':
                for entry in parents:
                    parent = str(prefix) + f".collision.{entry['control_parent']}"
                    compare(parent, parent + '.before.dat', parent + '.after.dat')
            else:
                for body in bodies:
                    capture = str(prefix) + f".{body['capture']}"
                    compare(capture, capture + '.before.dat', capture + '.after.dat',
                            body['before_tick'], body['after_tick'], body['saved_tick'],
                            capture + '.source.dat')
        print(f'{name}: original contracts and actual runtime captures pass', flush=True)
    print('Four keyboard-driven full bodies and 24 controlled recorder input parents match original compared RAM/display')


if __name__ == '__main__':
    main()
