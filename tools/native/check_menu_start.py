"""Exercise the complete native menu owner and input held during its pause."""
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
    oracle = ROOT / 'build/recomp/native_menu_start_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_menu_start_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-menu-start-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')

        def run(frames, name, replay=warmup):
            data = work / f'{name}.dat'
            result = subprocess.run([str(runner), '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--save-dir', str(work / name),
                                     '--data-out', str(data)], cwd=ROOT, check=True,
                                    capture_output=True, text=True, timeout=15)
            return json.loads(result.stdout), data

        _, initial = run(1, 'initial')
        subprocess.run([str(oracle), str(initial)], cwd=ROOT, check=True, timeout=15)
        first, _ = run(2270, 'pause-first')
        second, _ = run(2280, 'pause-second')
        assert first['screen'] == second['screen'] == 'menu', (first, second)
        for key in ('update_iterations', 'glyphs', 'record_updates', 'game_tick'):
            assert first[key] == second[key], (key, first, second)
        settled, path = run(2400, 'settled')
        assert settled['stage'] == 'C0FCB4', settled
        data = path.read_bytes()

        def field(address, length):
            offset = address if address < 0x80000 else address - 0xC00000 + 0x80000
            return data[offset:offset+length]

        assert field(0xC4588A, 1) == b'\0', 'source tone mute was overridden'
        assert field(0xC457D7, 1) == b'\1', 'source menu fade state differs'
        assert field(0xC4FF26, 4) == field(0xC45668, 4) == bytes.fromhex('001f0000')
        assert field(0xC458AD, 1) == b'\0'
        assert field(int.from_bytes(field(0xC45660, 4), 'big'), 64) == field(0xC08490, 64)
        for channel, slot in ((0, 13), (1, 14)):
            assert field(0xC4FE38+4*channel, 4) == field(0xC0A438+4*slot, 4)
        held = work / 'held.e9k'
        held.write_text(warmup.read_text() + 'F 2272 K 50 0 0 1\nF 2273 K 50 0 0 0\n')
        waiting, _ = run(2280, 'held-pause', held)
        assert waiting['input_queued'] == 2 and waiting['input_events'] == 0, waiting
        selected, _ = run(2310, 'held-selection', held)
        assert selected['mode'] == 1 and selected['stage'] == 'C0FECE', selected
        assert selected['input_queued'] == 0 and selected['input_events'] == 2, selected
        assert not selected['cpu_emulation'] and not selected['chipset_emulation'], selected
    print('Complete native menu setup, frozen pause, original volume/palette and queued selection pass')


if __name__ == '__main__':
    main()
