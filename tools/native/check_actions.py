"""Exercise native C12950 and compare state plus exact source sound arguments."""
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
    oracle = ROOT / 'build/recomp/native_actions_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_actions_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-actions-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 6600 K 61 0 0 0\n'
                          'F 6800 K 273 0 0 1\nF 6900 K 273 0 0 0\n')
        checkpoint = work / 'data.bin'
        result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
            '--save-dir', directory, '--headless', '--frames', '7000', '--replay', str(replay),
            '--data-out', str(checkpoint)], cwd=ROOT, check=True, capture_output=True, text=True, timeout=20)
        stats = json.loads(result.stdout)
        assert stats['stage'] == 'C10DAE', stats
        assert stats['control_frames'] > stats['hud_frames'] > 0, 'active/idle control owner not exercised'
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        data = checkpoint.read_bytes()
        def field(address, size):
            start = address - 0xC00000 + 0x80000
            return data[start:start + size]
        assert field(0xC458B0, 1) == b'\0', 'source control action not consumed'
        assert field(0xC457B8, 1) == b'\0', 'source event countdown not consumed'
        assert field(0xC45B54, 4) == bytes(4), 'source pending sound events not consumed'
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
        print(f"Native C12950 executes in {stats['control_frames']} active/idle frame passes")


if __name__ == '__main__':
    main()
