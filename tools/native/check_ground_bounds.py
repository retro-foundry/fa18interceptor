"""Check live startup's ground corner generation against original C2527C.

An independently executed original initializer checks every derived corner.
The omitted-initializer regression is recreated only in an oracle fixture.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/ground-bounds-startup')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    oracle = ROOT / 'build/recomp/native_model_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_model_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='ground-bounds-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        fixture = work / 'startup.dat'
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '1',
            '--save-dir', str(work / 'pilot'), '--data-out', str(fixture)],
            cwd=ROOT, capture_output=True, text=True, timeout=30)
        (args.out / 'native.log').write_text(result.stdout + result.stderr)
        assert result.returncode == 0, result.stderr
        stats = json.loads(result.stdout)
        assert not stats['cpu_emulation'] and not stats['chipset_emulation']
        assert stats['screen'] == 'splash' and stats['scene_frames'] == 0
        assert stats['update_iterations'] == 0 and stats['game_tick'] == 0
        data = fixture.read_bytes()
        assert len(data) == 0x100000
        def word(address):
            index = 0x80000 + address - 0xc00000
            return int.from_bytes(data[index:index+2], 'big', signed=True)
        records = []
        cursor = 0xc44880
        while word(cursor) >= 0:
            record = 0xc44880 + word(cursor)
            while word(record) != -1:
                records.append(record)
                record += 20
            cursor += 2
        assert records
        for mode in ('--setup-bounds', '--require-setup-bounds'):
            result = subprocess.run([str(oracle), str(fixture), mode], cwd=ROOT,
                capture_output=True, text=True, timeout=20)
            (args.out / (mode[2:] + '.log')).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, result.stdout + result.stderr
        omitted = bytearray(data)
        for record in records:
            index = 0x80000 + record + 8 - 0xc00000
            omitted[index:index+12] = bytes(12)
        rejected = work / 'omitted-startup.dat'
        rejected.write_bytes(omitted)
        result = subprocess.run([str(oracle), str(rejected), '--require-setup-bounds'],
            cwd=ROOT, capture_output=True, text=True, timeout=20)
        (args.out / 'omitted-startup-rejection.log').write_text(result.stdout + result.stderr)
        assert result.returncode != 0 and 'Startup did not generate original ground corner' in result.stderr
        report = dict(runner_sha256=hashlib.sha256(args.runner.read_bytes()).hexdigest(),
            startup_ram_sha256=hashlib.sha256(data).hexdigest(), native_run=stats,
            source_initializer='C0F50E -> C2527C -> C2574A',
            records=[f'{p:06X}' for p in records], corner_words_compared=len(records)*6,
            complete_non_stack_ram_matching=True, live_startup_corners_matching=True,
            omitted_initializer_rejected=True)
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'Live startup: all {len(records)} ground strip records match original corners; '
          'complete initializer RAM matches; missing startup is rejected')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
