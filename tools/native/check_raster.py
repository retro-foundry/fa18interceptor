"""Check connected native terrain and direct plane drawing against original bytes.

CPU/ROM and the chipset occur only in the independent validation executable.
Plane bytes exclude Copper fade; this does not establish whole-frame cadence,
object/cockpit rendering, or active flight.
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
    oracle = ROOT / 'build/recomp/native_raster_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
        '--main', 'tools/native/native_raster_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-raster-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
            'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
            'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
            'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
            'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n')
        outputs = []
        for frames, stage in ((5300, 'C10AE6'), (6100, 'C10C08')):
            checkpoint = work / f'{frames}.bin'
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                '--save-dir', directory, '--headless', '--frames', str(frames), '--replay', str(replay),
                '--data-out', str(checkpoint)], cwd=ROOT, check=True, capture_output=True, text=True, timeout=20)
            stats = json.loads(result.stdout)
            assert stats['stage'] == stage, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            assert stats['scene_frames'] > 100 and stats['terrain_polygons'] > 100, stats
            data = checkpoint.read_bytes()
            planes = [int.from_bytes(data[0x1000 + 4*i:0x1004 + 4*i], 'big') for i in range(4)]
            pixels = bytes(sum(((data[p + 40*y + x//8] >> (7-x%8)) & 1) << (3-i)
                for i, p in enumerate(planes)) for y in range(1, 144) for x in range(320))
            assert len(set(pixels)) >= 2, 'terrain preview is blank'
            outputs.append(pixels)
            subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=20)
        assert outputs[0] != outputs[1], 'camera/aircraft selection did not change the preview'
    print('Connected native setup terrain renders, changes with selection, and matches original raster buffers')

if __name__ == '__main__':
    main()
