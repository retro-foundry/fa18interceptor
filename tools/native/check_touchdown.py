"""Check carrier touchdown against an existing original consumed-input export.

Supply --game-input from fa18_recomp --game-input-out and --source-approach
from FA18_LOOP_DUMP=6289:path. This check does not repeat the reference replay.
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
    parser.add_argument('--game-input', type=Path, required=True)
    parser.add_argument('--source-approach', type=Path, required=True)
    args = parser.parse_args()
    assert args.game_input.read_text().splitlines()[0] == 'FA18_GAME_INPUT_V1'
    source = args.source_approach.read_bytes()
    assert len(source) == 0x100000, 'expected original RAM at start of iteration 6289'
    oracle = ROOT / 'build/recomp/native_touchdown_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_touchdown_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-touchdown-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        data = work / 'native.dat'
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '20000',
                                 '--input', str(args.game_input.resolve()), '--iterations', '6288',
                                 '--replay', str(warmup), '--save-dir', str(work / 'pilot'), '--data-out', str(data)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
        stats = json.loads(result.stdout)
        assert stats['replay_iterations'] == 6288 and stats['stage'] == 'C10DAE', stats
        assert not stats['input_queued'] and not stats['postflight_resets'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        native = data.read_bytes()
        root = 0xC46184 - 0xC00000 + 0x80000
        assert native[root + 2:root + 4] == b'\xc0\x82', 'carrier ground/contact flags differ'
        assert native[root + 24:root + 28] == b'\0\0\x77\x08', 'not at carrier deck height'
        assert native[root + 108:root + 112] == b'\0\0\0\0', 'aircraft has not stopped'
        assert source[root + 4] ^ native[root + 4] == 4, 'existing region flag difference changed'
        countdown = lambda ram: int.from_bytes(ram[root + 76:root + 78], 'big')
        assert (countdown(native) - countdown(source)) & 0xffff == 16
        for offset in range(164):
            if offset not in (4, 76, 77):
                assert source[root + offset] == native[root + offset], f'aircraft +{offset:02X} differs'
        subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=15)
    print('Native carrier touchdown matches original aircraft motion/contact/reset state; eight mission-reset cases pass')


if __name__ == '__main__':
    main()
