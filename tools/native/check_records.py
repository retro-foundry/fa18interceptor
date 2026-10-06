"""Compare native view/control and record updates with original instructions.

This bounded component check uses CPU/ROM only in the validation oracle.
It covers startup, grounded throttle motion and stick recording, not full
flight/frame parity.
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
    parser.add_argument('--reference', type=Path, help='optional original RAM checkpoint to exercise active record children')
    args = parser.parse_args()
    oracle = ROOT / 'build/recomp/native_records_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_records_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-records-', dir=ROOT / 'build') as directory:
        checkpoint = Path(directory) / 'bootstrap.bin'
        command = [str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                        '--save-dir', directory, '--headless', '--frames', '1',
                        '--data-out', str(checkpoint)]
        subprocess.run(command, cwd=ROOT, check=True, timeout=15)
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
        replay = Path(directory) / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n')
        command[command.index('--frames') + 1] = '4000'
        result = subprocess.run(command + ['--replay', str(replay)], cwd=ROOT,
                                check=True, capture_output=True, text=True, timeout=15)
        assert json.loads(result.stdout)['record_updates'] > 1, 'ongoing record path not exercised'
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
        replay.write_text(replay.read_text() + 'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
            'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
            'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n')
        for frames, stage in (('5450', 'C10C08'), ('6100', 'C10DAE')):
            command[command.index('--frames') + 1] = frames
            result = subprocess.run(command + ['--replay', str(replay)], cwd=ROOT,
                check=True, capture_output=True, text=True, timeout=15)
            assert json.loads(result.stdout)['stage'] == stage, result.stdout
            subprocess.run([str(oracle), str(checkpoint), frames], cwd=ROOT, check=True, timeout=15)
        def field(address, length):
            data = checkpoint.read_bytes()
            start = address - 0xC00000 + 0x80000
            return data[start:start + length]
        initial_position = field(0xC46198, 12)
        replay.write_text(replay.read_text() + 'F 6200 K 61 0 0 1\nF 6600 K 61 0 0 0\n'
                          'F 6800 K 273 0 0 1\nF 6900 K 273 0 0 0\n')
        # C25312 limits game updates using the disk table. Allow enough game
        # updates for C1B27E's unchanged one-step ramp to reach its limit 20.
        for frames in ('6500', '6880', '7000'):
            command[command.index('--frames') + 1] = frames
            result = subprocess.run(command + ['--replay', str(replay)], cwd=ROOT,
                check=True, capture_output=True, text=True, timeout=15)
            assert json.loads(result.stdout)['stage'] == 'C10DAE', result.stdout
            if frames == '6500':
                assert field(0xC46198, 12) != initial_position, 'throttle did not move aircraft'
                assert int.from_bytes(field(0xC461F2, 2), 'big') > 0, 'throttle produced no speed'
                assert field(0xC461C2, 4) != bytes(4), 'horizontal motion was not published'
            elif frames == '6880':
                assert field(0xC461E9, 1)[0] & 0x30 == 0x10, 'stick press not recorded'
                assert field(0xC461AC, 1) == bytes([20]), 'stick response did not ramp'
            else:
                assert field(0xC461E9, 1)[0] & 0x30 == 0, 'stick release not recorded'
                assert field(0xC461AC, 1) == bytes(1), 'stick response did not release'
            subprocess.run([str(oracle), str(checkpoint), frames], cwd=ROOT, check=True, timeout=15)
    if args.reference:
        subprocess.run([str(oracle), str(args.reference.resolve())], cwd=ROOT, check=True, timeout=15)

if __name__ == '__main__':
    main()
