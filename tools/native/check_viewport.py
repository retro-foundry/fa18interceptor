"""Check source viewport/fade sequences and PAL updates during a display wait."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    runner = parser.parse_args().runner.resolve()
    oracle = ROOT / 'build/recomp/native_viewport_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_viewport_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-viewport-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        snapshots = []
        for frames in (3001, 3002):
            path = work / f'{frames}.dat'
            result = subprocess.run([str(runner), '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--save-dir', str(work / str(frames)),
                                     '--data-out', str(path)], cwd=ROOT, check=True,
                                    capture_output=True, text=True, timeout=15)
            snapshots.append((json.loads(result.stdout), path.read_bytes()))
            if frames == 3001:
                subprocess.run([str(oracle), str(path)], cwd=ROOT, check=True, timeout=15)
        first, second = snapshots
        assert first[0]['display_pending'] and second[0]['display_pending'], snapshots
        for key in ('update_iterations', 'record_updates', 'hud_frames', 'glyphs', 'game_tick'):
            assert first[0][key] == second[0][key], (key, first[0], second[0])
        address = 0xC4FF26-0xC00000+0x80000
        volume = [int.from_bytes(data[address:address+4], 'big') for _, data in snapshots]
        assert volume == [0x1F0000, 0x1EC000], volume
        assert not second[0]['cpu_emulation'] and not second[0]['chipset_emulation'], second[0]
    print('Viewport/fade matches source and advances on the PAL clock while the game waits')


if __name__ == '__main__':
    main()
