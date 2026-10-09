"""Exercise connected setup scenery/aircraft against original code.

Copper fade is excluded: compare plane bytes, transformed vertices and returns.
CPU/ROM/chipset dependencies belong solely to the independent oracle executable.
Also compare all non-stack RAM for complete grid, control and followup parents.
This checks setup integration, not flight cadence or cockpit/HUD integration.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    args = parser.parse_args()
    oracle = ROOT / 'build/recomp/native_model_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_model_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-models-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n')
        for frames, stage in ((4000, 'C1075A'), (5300, 'C10AE6'), (6100, 'C10DAE')):
            checkpoint = work / f'{frames}.bin'
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', directory, '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--data-out', str(checkpoint)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            stats = json.loads(result.stdout)
            assert stats['stage'] == stage and stats['model_calls'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            subprocess.run([str(oracle), str(checkpoint), '--require-setup-bounds'], cwd=ROOT, check=True, timeout=20)
            subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=20)
            if frames==6100:
                # The corrected cold setup need not display an aircraft's
                # C22AC0 descriptor. Require its inactive contract explicitly
                # using an actual disk-started aircraft/ship and descriptor.
                subprocess.run([str(oracle), str(checkpoint), '--inactive-only'],
                               cwd=ROOT, check=True, timeout=20)
            print(f'{frames}: {stats["model_calls"]} connected descriptor calls; stage {stage}')
    print('Native setup scenery, aircraft and scene parents match focused original comparisons')


if __name__ == '__main__':
    main()
