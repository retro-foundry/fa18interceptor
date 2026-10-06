"""Check native cleanup, overlays and scene labels without an original game replay."""
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
    oracles = []
    for name in ('frame_tail', 'frame_labels'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles.append(oracle)
    with tempfile.TemporaryDirectory(prefix='native-frame-tail-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        data = work / '2400.dat'
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
            '--input', str(ROOT / 'captures/native/demo01/input.fa18in'), '--iterations', '2400',
            '--replay', str(warmup), '--save-dir', str(work / 'pilot'), '--data-out', str(data)],
            cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
        stats = json.loads(result.stdout)
        assert stats['stage'] == 'C10DAE' and stats['hud_frames'] > 0, stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        # At this recorded-update boundary the timer is pending, after the
        # new cleanup and before the counter/overlay/final-message tail.
        marker = int.from_bytes(data.read_bytes()[0xC45AD4-0xC00000+0x80000:0xC45AD6-0xC00000+0x80000], 'big')
        assert stats['timer_pending'] and marker == 0x1D4, (stats, marker)
        for oracle in oracles:
            subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=20)
        print(f'{stats["hud_frames"]} native HUD passes reached source selection cleanup before yielding')
    print('Native cleanup/overlays/scene labels pass; stores/grid markers and frame parity remain open')


if __name__ == '__main__':
    main()
