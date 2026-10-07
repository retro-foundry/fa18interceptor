"""Verify keyboard/recorder countermeasures and original gameplay contracts.

The integration entry shares the playable runner's runtime objects and starts
from disk/input. Controlled input/collision parents follow ordinary Free Flight.
No original full replay is run; complete body comparisons retain HUD pixels.
"""
import argparse
import json
import os
import re
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
        raise RuntimeError(run.stderr or run.stdout or f'Native integration exited with code {run.returncode}')
    exports = [json.loads(line) for line in run.stdout.splitlines()]
    bodies = [entry for entry in exports if 'capture' in entry]
    parents = [entry for entry in exports if 'control_parent' in entry]
    fd_inputs = [entry for entry in exports if 'fd_input' in entry]
    pending_inputs = [entry for entry in exports if 'pending_input' in entry]
    message_bodies = [entry for entry in exports if 'message_body' in entry]
    clear_bodies = [entry for entry in exports if 'clear_body' in entry]
    hud_bodies = [entry for entry in exports if 'hud_body' in entry]
    debug_bodies = [entry for entry in exports if 'debug_body' in entry]
    assert [body['capture'] for body in bodies] == list(range(4)), bodies
    assert [entry['control_parent'] for entry in parents] == list(range(4)), parents
    assert any(entry['collision_hit'] for entry in parents), parents
    assert [entry['fd_input'] for entry in fd_inputs] == [0, 1], fd_inputs
    assert sorted(entry['pending_input'] for entry in pending_inputs) == list(range(72)), pending_inputs
    assert [entry['message_body'] for entry in message_bodies] == list(range(15)), message_bodies
    assert [entry['assigned'] for entry in message_bodies] == [True] * 13 + [False] * 2, message_bodies
    assert {entry['input_byte'] & 0x80 for entry in message_bodies[:12]} == {0, 0x80}
    assert [entry['clear_body'] for entry in clear_bodies] == list(range(12)), clear_bodies
    assert all(entry['return_owner'] == 2 and entry['input_byte'] == 0 and
        entry['saved_tick'] & 31 == 8 for entry in clear_bodies), clear_bodies
    assert sum(entry['chain'] for entry in pending_inputs) == 12, pending_inputs
    assert [entry['hud_body'] for entry in hud_bodies] == list(range(12)), hud_bodies
    assert all(entry['return_owner'] in (3, 4, 5) and entry['saved_tick'] & 31 != 8
        for entry in hud_bodies), hud_bodies
    assert {entry['return_owner'] for entry in hud_bodies} == {3, 4, 5}, hud_bodies
    assert [entry['debug_body'] for entry in debug_bodies] == list(range(12)), debug_bodies
    assert all(entry['return_owner'] == 6 for entry in debug_bodies), debug_bodies
    assert {entry['recorder_mode'] for entry in pending_inputs} == {1, 2, 3}, pending_inputs
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    source_carries = {}
    for name in ('frame_body', 'input', 'control_effects'):
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
            def compare(capture, *values, environment=None):
                comparison = subprocess.run([str(oracle), *map(str, values)],
                    cwd=ROOT, capture_output=True, text=True, timeout=15, env=environment)
                log.write(comparison.stdout + comparison.stderr)
                log.flush()
                if comparison.returncode:
                    retain_failure(capture, work)
                    raise RuntimeError(comparison.stderr or comparison.stdout)
                return comparison.stdout
            if name != 'frame_body':
                compare(str(prefix) + '.0', str(prefix) + '.0.before.dat')
            if name == 'input':
                for entry in fd_inputs:
                    parent = str(prefix) + f".fd.{entry['fd_input']}"
                    compare(parent, parent + '.before.dat', parent + '.after.dat', entry['raw'])
                for entry in pending_inputs:
                    parent = str(prefix) + f".pending.{entry['pending_input']}"
                    carry = [source_carries[entry['pending_input']]] if entry['pending_input'] >= 24 else []
                    compare(parent, parent + '.before.dat', parent + '.after.dat', 'pending', *carry)
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
                for body in message_bodies:
                    capture = str(prefix) + f".message.{body['message_body']}"
                    environment = dict(os.environ)
                    environment.pop('FA18_FRAME_EXPECT_INPUT_CARRY', None)
                    if body['assigned']:
                        environment['FA18_FRAME_EXPECT_INPUT_CARRY'] = str(body['input_byte'])
                    output = compare(capture, capture + '.before.dat', capture + '.after.dat',
                        body['before_tick'], body['after_tick'], body['saved_tick'],
                        capture + '.source.dat', environment=environment)
                    source_carries[24 + body['message_body']] = int(re.search(r'Frame input carry: (\d+)', output)[1])
                for body in clear_bodies:
                    capture = str(prefix) + f".clear.{body['clear_body']}"
                    environment = dict(os.environ, FA18_FRAME_EXPECT_INPUT_CARRY=str(body['input_byte']))
                    output = compare(capture, capture + '.before.dat', capture + '.after.dat',
                        body['before_tick'], body['after_tick'], body['saved_tick'],
                        capture + '.source.dat', environment=environment)
                    source_carries[36 + body['clear_body']] = int(re.search(r'Frame input carry: (\d+)', output)[1])
                for body in hud_bodies:
                    capture = str(prefix) + f".hud.{body['hud_body']}"
                    environment = dict(os.environ, FA18_FRAME_EXPECT_INPUT_CARRY=str(body['input_byte']))
                    output = compare(capture, capture + '.before.dat', capture + '.after.dat',
                        body['before_tick'], body['after_tick'], body['saved_tick'],
                        capture + '.source.dat', environment=environment)
                    source_carries[48 + body['hud_body']] = int(re.search(r'Frame input carry: (\d+)', output)[1])
                for body in debug_bodies:
                    capture = str(prefix) + f".debug.{body['debug_body']}"
                    environment = dict(os.environ, FA18_FRAME_EXPECT_INPUT_CARRY=str(body['input_byte']))
                    output = compare(capture, capture + '.before.dat', capture + '.after.dat',
                        body['before_tick'], body['after_tick'], body['saved_tick'],
                        capture + '.source.dat', environment=environment)
                    source_carries[60 + body['debug_body']] = int(re.search(r'Frame input carry: (\d+)', output)[1])
        print(f'{name}: original contracts and actual runtime captures pass', flush=True)
    print('55 full bodies and 72 recorder input parents match original RAM/display, including message, page-clear, HUD and debug text first-depleted returns')


if __name__ == '__main__':
    main()
