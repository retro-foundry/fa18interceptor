"""Complete native carrier qualification using existing original game-input evidence.

Supply --game-input from the original reference export and --source-end from
its --ram-out. This check reuses evidence and never repeats the reference replay.
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
    parser.add_argument('--source-end', type=Path, required=True)
    args = parser.parse_args()
    assert args.game_input.read_text().splitlines()[0] == 'FA18_GAME_INPUT_V1'
    source = args.source_end.read_bytes()
    assert len(source) == 0x100048, 'expected original RAM/register export'
    offset = lambda address: address if address < 0x80000 else address - 0xC00000 + 0x80000
    source_log = int.from_bytes(source[offset(0xC1AB74):offset(0xC1AB74) + 4], 'big')
    assert source[offset(source_log):offset(source_log) + 2] == b'\0\1', 'original qualification not successful'
    oracle = ROOT / 'build/recomp/native_qualification_result_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_qualification_result_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-qualification-result-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        data, pilot = work / 'native.dat', work / 'pilot'
        runner = str(args.runner.resolve())
        result = subprocess.run([runner, '--headless', '--frames', '30000',
                                 '--input', str(args.game_input.resolve()), '--iterations', '8038',
                                 '--replay', str(warmup), '--save-dir', str(pilot), '--data-out', str(data)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
        stats = json.loads(result.stdout)
        assert stats['replay_iterations'] == 8038 and stats['replay_events'] == 554, stats
        assert stats['mode'] == 9 and stats['stage'] == 'C10DAE' and stats['screen'] == 'scene-setup', stats
        assert not stats['input_queued'] and not stats['postflight_resets'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        native = data.read_bytes()
        root = offset(0xC46184)
        # After the result/restart, the stable aircraft pose/contact/speed and
        # matrices agree. Full state/timing after the result remains open.
        for start, end in ((0, 38), (98, 100), (102, 114), (128, 164)):
            assert source[root + start:root + end] == native[root + start:root + end], f'aircraft +{start:02X} differs'
        saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78 and saved[:2] == b'\0\1', 'qualification marker was not persisted'
        native_log = int.from_bytes(native[offset(0xC1AB74):offset(0xC1AB74) + 4], 'big')
        assert saved == native[offset(native_log):offset(native_log) + 78], 'save differs from native pilot record'
        subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=15)
        reloaded = work / 'reloaded.dat'
        subprocess.run([runner, '--headless', '--frames', '1', '--save-dir', str(pilot), '--data-out', str(reloaded)],
                       cwd=ROOT, check=True, capture_output=True, text=True, timeout=10)
        loaded = reloaded.read_bytes()
        assert loaded[offset(native_log):offset(native_log) + 78] == saved, 'qualification save did not reload'
    print('Native carrier qualification completes, persists success, restarts and reloads its log; 77 complete result/config-parent cases pass')


if __name__ == '__main__':
    main()
