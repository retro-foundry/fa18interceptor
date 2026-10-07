"""Compare native flight with the raw keys actually consumed by the original game."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    args = parser.parse_args()
    reference = ROOT / 'build/recomp/fa18_recomp.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(reference.relative_to(ROOT))],
                   cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-game-input-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        recording = ROOT / 'captures/native/qual_carrier_success/input.fa18in'
        consumed = work / 'consumed.fa18in'
        base = [str(reference), '--state', str(recording.with_name('state.bin')),
                '--rom', str(ROOT / 'local/system/kick13.rom'), '--ports', 'off',
                '--input', str(recording), '--frames', '6080']
        for extra, message in ((['--no-recomp'], 'translated main-loop boundaries'),
                               (['--ports', 'on'], 'requires --ports off')):
            rejected = work / 'rejected.fa18in'
            result = subprocess.run(base + extra + ['--game-input-out', str(rejected)], cwd=ROOT,
                                    capture_output=True, text=True, timeout=10)
            assert result.returncode == 2 and message in result.stderr, result.stderr
            assert not rejected.exists(), 'invalid reference mode created input evidence'
        results = []
        snapshots = []
        for name, extra in (('baseline', []), ('export', ['--game-input-out', str(consumed)])):
            data, checkpoint = work / f'{name}.dat', work / f'{name}-5509.dat'
            env = dict(os.environ, FA18_LOOP_DUMP=f'5509:{checkpoint}')
            result = subprocess.run(base + ['--ram-out', str(data)] + extra, cwd=ROOT, env=env,
                                    check=True, capture_output=True, text=True, timeout=60)
            results.append((json.loads(result.stdout), data.read_bytes()))
            snapshots.append(checkpoint.read_bytes())
        assert results[0] == results[1] and snapshots[0] == snapshots[1], 'export changed original execution'
        lines = consumed.read_text().splitlines()
        assert lines[0] == 'FA18_GAME_INPUT_V1' and lines[-1].startswith('end '), lines
        events = [line.split() for line in lines[1:-1]]
        injected = [line.split() for line in recording.read_text().splitlines()[1:]
                    if line.split()[0].isdigit() and int(line.split()[0]) <= 5508]
        game_edges = [(int(e[0]), int(e[3]), int(e[4])) for e in events if int(e[0]) <= 5508]
        injected_edges = [(int(e[0]), int(e[3]), int(e[4])) for e in injected]
        assert game_edges != injected_edges, 'probe no longer exercises injection/consumption difference'
        assert game_edges.count((5508, 32, 1)) == 1, 'resumed hook command exported twice'

        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        native_data = work / 'native.dat'
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '12000',
                                 '--input', str(consumed), '--iterations', '5508', '--replay', str(warmup),
                                 '--save-dir', str(work / 'pilot'), '--data-out', str(native_data)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
        stats = json.loads(result.stdout)
        assert stats['replay_iterations'] == 5508 and stats['replay_events'] == len(game_edges), stats
        assert stats['stage'] == 'C10DAE' and not stats['input_queued'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        native, source = native_data.read_bytes(), snapshots[1]
        root = 0xC46184 - 0xC00000 + 0x80000
        # C08EE4/C08EB8 cold startup now constructs the source aircraft kind.
        # Require the complete core, including its region flag and countdown.
        for offset in range(164):
            assert source[root + offset] == native[root + offset], f'aircraft +{offset:02X} differs'
        for address in (0xC4582E, 0xC45830):
            offset = address - 0xC00000 + 0x80000
            assert source[offset] == native[offset], f'stick {address:06X} differs'
    print('Game-input export preserves original RAM/registers/timing; native turn input and aircraft state agree')


if __name__ == '__main__':
    main()
