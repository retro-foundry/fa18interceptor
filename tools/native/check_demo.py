"""Run native disk-backed demonstration flight and focused original-code oracles.

Copper fade is excluded; complete recorded frame parity is not claimed.
Optional existing reference checkpoints add disk-buffer and source-state coverage
without rerunning the expensive original recording.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def field(data, address, count):
    start = address if address < 0x80000 else address - 0xC00000 + 0x80000
    return data[start:start + count]


def value(data, address, count):
    return int.from_bytes(field(data, address, count), 'big')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--map', type=Path, default=ROOT / 'build/native-cmake/native/fa18_native.map')
    parser.add_argument('--source-initial', type=Path)
    parser.add_argument('--source-checkpoint', type=Path)
    args = parser.parse_args()
    adf = ROOT / 'local/media/fa18.adf'
    seal = hashlib.sha256(adf.read_bytes()).digest()
    symbols = args.map.read_text(errors='replace')
    assert not re.search(r'm68ki?_|fa18_(?:bus_|machine_|recomp_)|glue_|custom_write|wait_blitter', symbols, re.I)
    oracles = {}
    for name in ('demo', 'records', 'model'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = oracle
    with tempfile.TemporaryDirectory(prefix='native-demo-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        initial = work / 'initial.dat'
        subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '1', '--adf', str(adf),
                        '--save-dir', str(work / 'initial-pilot'), '--data-out', str(initial)],
                       cwd=ROOT, check=True, capture_output=True, text=True, timeout=10)
        baseline = initial.read_bytes()
        assert any(field(baseline, value(baseline, 0xC4FDA4, 4), 3676)), 'disk recorder bytes missing'
        assert any(field(baseline, value(baseline, 0xC4FDB0, 4), 14704)), 'disk recorder words missing'
        if args.source_initial:
            reference = args.source_initial.read_bytes()
            for pointer, length in ((0xC4FDA4, 3676), (0xC4FDB0, 14704)):
                assert field(baseline, value(baseline, pointer, 4), length) == field(
                    reference, value(reference, pointer, 4), length), 'original disk recorder asset mismatch'
        for iterations in (2000, 2400, 3000, 4892):
            data_path = work / f'{iterations}.dat'
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                '--adf', str(adf), '--save-dir', str(work / f'pilot-{iterations}'),
                '--input', str(ROOT / 'captures/native/demo01/input.fa18in'), '--iterations', str(iterations),
                '--replay', str(warmup), '--data-out', str(data_path)],
                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
            stats = json.loads(result.stdout)
            assert stats['mode'] == 3 and stats['screen'] == 'scene-setup', stats
            assert stats['replay_iterations'] == iterations and stats['replay_events'] == 2, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            assert stats['record_updates'] > 1 and stats['model_calls'] > 0, stats
            assert stats['input_queued'] == 0, stats
            data = data_path.read_bytes()
            assert value(data, 0xC4584B, 1) == 3, 'demo recorder mode'
            if iterations >= 2400:
                assert stats['stage'] == 'C10DAE' and value(data, 0xC461F2, 2) > 0, 'demo aircraft did not fly'
                assert field(data, 0xC46198, 12) != field(baseline, 0xC46198, 12), 'no root motion'
            if iterations == 4892:
                assert value(data, 0x2000 + 62, 2) > value(baseline, 0x2000 + 62, 2), 'recorded launches absent'
            subprocess.run([str(oracles['records']), str(data_path)], cwd=ROOT, check=True, timeout=15)
            if iterations == 2400:
                subprocess.run([str(oracles['demo']), str(data_path)], cwd=ROOT, check=True, timeout=15)
            if iterations == 4892:
                subprocess.run([str(oracles['model']), str(data_path)], cwd=ROOT, check=True, timeout=15)
            print(f'{iterations}: native disk-backed demo {stats["stage"]}; {stats["record_updates"]} record updates')
        if args.source_checkpoint:
            subprocess.run([str(oracles['records']), str(args.source_checkpoint.resolve())],
                           cwd=ROOT, check=True, timeout=15)
    assert hashlib.sha256(adf.read_bytes()).digest() == seal, 'original disk changed'
    print('Native demo flight and focused source comparisons pass; full frame timing remains open')


if __name__ == '__main__':
    main()
