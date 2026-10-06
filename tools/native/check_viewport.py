"""Check complete source input PAL callbacks, host counters and display waits."""
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
        offset = lambda address: address - 0xC00000 + 0x80000
        word = lambda data, address: int.from_bytes(data[offset(address):offset(address) + 2], 'big', signed=True)
        assert first[0]['display_pending'] and second[0]['display_pending'], snapshots
        for key in ('update_iterations', 'record_updates', 'hud_frames', 'glyphs', 'game_tick'):
            assert first[0][key] == second[0][key], (key, first[0], second[0])
        address = 0xC4FF26-0xC00000+0x80000
        volume = [int.from_bytes(data[address:address+4], 'big') for _, data in snapshots]
        assert volume == [0x1F0000, 0x1EC000], volume
        assert (word(second[1], 0xC45774) - word(first[1], 0xC45774)) & 65535 == 1
        assert [word(second[1], address) for address in (0xC081AC, 0xC081AE, 0xC081B0, 0xC081B2)] == [-960, -960, 960, 960]
        assert not second[0]['cpu_emulation'] and not second[0]['chipset_emulation'], second[0]
        mouse = work / 'mouse.e9k'
        mouse.write_text('E9K_INPUT_V1\nF 0 m 0 127 -1 0\nF 1 m 0 129 2 0\n'
                         'F 2 m 4 -129 -129 0\nF 3 m 4 128 128 0\n')
        # Cold bootstrap supplies throttle 72 and PLAYER_READY=1; the source
        # negative-Y halving branch is separately covered by the full oracle.
        for frames, expected in enumerate(((127, 73), (0, 71), (127, -56), (-1, 72)), start=1):
            path = work / f'mouse-{frames}.dat'
            result = subprocess.run([str(runner), '--headless', '--frames', str(frames),
                                     '--replay', str(mouse), '--save-dir', str(work / 'mouse-pilot'),
                                     '--data-out', str(path)], cwd=ROOT, check=True,
                                    capture_output=True, text=True, timeout=10)
            data = path.read_bytes()
            actual = (word(data, 0xC45776), word(data, 0xC45778))
            assert actual == expected, (frames, actual, expected)
            assert word(data, 0xC45774) == frames
            assert not json.loads(result.stdout)['cpu_emulation']
        mouse.write_text('E9K_INPUT_V1\n' + ''.join(f'F {frame} m 0 127 127 0\n' for frame in range(20)))
        path = work / 'mouse-limits.dat'
        subprocess.run([str(runner), '--headless', '--frames', '20', '--replay', str(mouse),
                        '--save-dir', str(work / 'mouse-pilot'), '--data-out', str(path)],
                       cwd=ROOT, check=True, capture_output=True, text=True, timeout=10)
        assert (word(path.read_bytes(), 0xC45776), word(path.read_bytes(), 0xC45778)) == (960, -960)
        mouse.write_text('E9K_INPUT_V1\nF 0 m 1 0 0 0\n')
        result = subprocess.run([str(runner), '--headless', '--frames', '1', '--replay', str(mouse)],
                                cwd=ROOT, capture_output=True, text=True, timeout=10)
        assert result.returncode and 'unsupported replay row' in result.stderr
        buttons = work / 'buttons.e9k'
        events = ((1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49))
        buttons.write_text('E9K_INPUT_V1\n' + ''.join(
            f'F {frame} K {key} 0 0 1\nF {frame + 2} K {key} 0 0 0\n' for frame, key in events) +
            'F 6200 b 0 0 1 0\nF 6220 b 4 0 0 0\nF 6240 b 4 1 1 0\nF 6260 b 0 1 0 0\n')
        for frames, expected in ((6210, 1), (6230, 0), (6250, 2), (6270, 0)):
            path = work / f'buttons-{frames}.dat'
            result = subprocess.run([str(runner), '--headless', '--frames', str(frames),
                                     '--replay', str(buttons), '--save-dir', str(work / 'button-pilot'),
                                     '--data-out', str(path)], cwd=ROOT, check=True,
                                    capture_output=True, text=True, timeout=15)
            stats = json.loads(result.stdout)
            assert stats['stage'] == 'C10DAE' and stats['hud_frames'] > 0, stats
            assert word(path.read_bytes(), 0xC4577E) == expected, (frames, expected)
    print('Complete input/viewport/fade matches source; actual mouse wrap/clamp, button polling and PAL updates during waits pass')


if __name__ == '__main__':
    main()
