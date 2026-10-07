"""Verify keyboard countermeasures and source launch/motion/drawing contracts.

The integration entry shares the playable runner's runtime objects and starts
from disk/input. Only the opcode oracle varies records for branch coverage.
No original full replay is run; complete body comparisons retain HUD pixels.
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_countermeasures_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/countermeasure-check')
    args = parser.parse_args()
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    prefix = work / 'frame'
    run = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                          str(work / 'pilot'), str(prefix)], cwd=ROOT, check=True,
                         capture_output=True, text=True, timeout=25)
    exports = [json.loads(line) for line in run.stdout.splitlines()]
    bodies = [entry for entry in exports if 'capture' in entry]
    parents = [entry for entry in exports if 'control_parent' in entry]
    fd_inputs = [entry for entry in exports if 'fd_input' in entry]
    assert [body['capture'] for body in bodies] == list(range(4)), bodies
    assert [entry['control_parent'] for entry in parents] == list(range(4)), parents
    assert any(entry['collision_hit'] for entry in parents), parents
    assert [entry['fd_input'] for entry in fd_inputs] == [0, 1], fd_inputs
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    for name in ('input', 'control_effects', 'frame_body'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        # These reference builds share an object directory: keep sequential.
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        if name != 'frame_body':
            subprocess.run([str(oracle), str(prefix) + '.0.before.dat'],
                           cwd=ROOT, check=True, timeout=15)
            if name == 'input':
                for entry in fd_inputs:
                    parent = str(prefix) + f".fd.{entry['fd_input']}"
                    subprocess.run([str(oracle), parent + '.before.dat', parent + '.after.dat', str(entry['raw'])],
                                   cwd=ROOT, check=True, timeout=15)
            if name == 'control_effects':
                for entry in parents:
                    parent = str(prefix) + f".collision.{entry['control_parent']}"
                    subprocess.run([str(oracle), parent + '.before.dat', parent + '.after.dat'],
                                   cwd=ROOT, check=True, timeout=15)
            continue
        for body in bodies:
            capture = Path(str(prefix) + f".{body['capture']}")
            subprocess.run([str(oracle), str(capture) + '.before.dat',
                            str(capture) + '.after.dat', str(body['before_tick']),
                            str(body['after_tick']), str(body['saved_tick']),
                            str(capture) + '.source.dat'], cwd=ROOT, check=True, timeout=15)
    print('Keyboard flare/chaff launches and early ground-contact expiry: four actual native bodies match original gameplay and display')


if __name__ == '__main__':
    main()
