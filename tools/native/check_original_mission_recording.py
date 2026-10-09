"""Generate an original mission recording, then replay it without the pilot.

The external validation pilot chooses ordinary physical keyboard edges only.
Original replay must reproduce every trace byte and final RAM before evidence
is accepted. A failed pilot route remains a failed route, never mission success.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer
from compare_flight_traces import read_trace, number


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--reuse-driver', action='store_true', help='verify an already retained complete driver recording')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    recording = ROOT / 'captures/native/qual_carrier_success/input.fa18in'
    media = (recording, recording.with_name('state.bin'), ROOT / 'local/system/kick13.rom')
    hashes = {str(p.relative_to(ROOT)): digest(p.read_bytes()) for p in media}
    seal = json.loads(recording.with_name('run.json').read_text())
    assert digest(recording.read_bytes()) == seal['input_sha256'], 'sealed input changed'
    assert digest(recording.with_name('state.bin').read_bytes()) == seal['start_state']['sha256']
    assert digest((ROOT / 'local/system/kick13.rom').read_bytes()) == seal['rom_sha256']
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_PILOT_'))}
    with tempfile.TemporaryDirectory(prefix='original-mission-recording-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        def run(executable, input_path, log_path, trace_path, ram_path, consumed_path, extra_env=None):
            with log_path.open('w') as log:
                result = subprocess.run([str(executable), '--state', str(recording.with_name('state.bin')),
                    '--rom', str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input', str(input_path),
                    '--to-end', '--frames', '40000', '--game-input-out', str(consumed_path),
                    '--ram-out', str(ram_path)], cwd=ROOT,
                    env=dict(env, FA18_LOOP_TRACE=str(trace_path), **(extra_env or {})),
                    stdout=log, stderr=subprocess.STDOUT, timeout=300)
            return result.returncode
        if not args.reuse_driver:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output', 'build/recomp/fa18_original_mission_pilot.exe',
                '--replace-source', 'port/recomp/loop_input.c=tools/native/original_mission_pilot_loop.c'], cwd=ROOT, check=True)
            code = run(ROOT / 'build/recomp/fa18_original_mission_pilot.exe', recording, args.out / 'driver.log',
                work / 'driver.jsonl', work / 'driver.dat', args.out / 'consumed.fa18in',
                dict(FA18_ORIGINAL_PILOT_INPUT=str((args.out / 'input.fa18in').resolve()),
                     FA18_ORIGINAL_PILOT_KEYS=str((args.out / 'controller-choices.e9k').resolve())))
            assert code in (0, 1), f'original pilot process failed: {code}'
            for name in ('driver.jsonl', 'driver.dat'):
                (args.out / f'{name}.gz').write_bytes(gzip.compress((work / name).read_bytes(), mtime=0))
            # Successful footer and complete final RAM are required even for a
            # diagnosed failed route. Truncated/time-limited output is not proof.
            _, rows = read_trace(work / 'driver.jsonl')
            ram = (work / 'driver.dat').read_bytes()
            log = (args.out / 'driver.log').read_text()
            preliminary = dict(input_hashes=hashes, driver_returncode=code,
                driver_trace_sha256=digest((work / 'driver.jsonl').read_bytes()),
                driver_final_ram_sha256=digest(ram), generated_input_sha256=digest((args.out / 'input.fa18in').read_bytes()),
                consumed_input_sha256=digest((args.out / 'consumed.fa18in').read_bytes()),
                mission_success=code == 0 and 'complete and menu returned' in log,
                original_outcome='crash/reset' if 'Original crash/reset outcome' in log else
                                 'player destroyed' if 'Original player destroyed' in log else 'success' if code == 0 else 'incomplete',
                final_phase=integer(ram, 0xC45798, 1), final_mode=integer(ram, 0xC458A6, 1),
                final_completions=integer(ram, integer(ram, 0xC1AB74, 4) + 56, 2),
                iterations=max(rows))
            (args.out / 'report.json').write_text(json.dumps(preliminary, indent=2) + '\n')
            print(f'Original driver recording closed: {max(rows)} boundaries; outcome {preliminary["original_outcome"]}', flush=True)
        report = json.loads((args.out / 'report.json').read_text())
        assert report['input_hashes'] == hashes, 'original media changed'
        data = gzip.decompress((args.out / 'driver.jsonl.gz').read_bytes())
        assert digest(data) == report['driver_trace_sha256']
        assert digest(gzip.decompress((args.out / 'driver.dat.gz').read_bytes())) == report['driver_final_ram_sha256']
        assert digest((args.out / 'input.fa18in').read_bytes()) == report['generated_input_sha256']
        assert digest((args.out / 'consumed.fa18in').read_bytes()) == report['consumed_input_sha256']
        _, rows = read_trace(args.out / 'driver.jsonl.gz')
        result = run(ROOT / 'build/recomp/fa18_recomp.exe', args.out / 'input.fa18in', args.out / 'replay.log',
            work / 'replay.jsonl', work / 'replay.dat', work / 'replay-consumed.fa18in')
        assert result == 0, 'unmodified original replay did not complete'
        replay_trace = (work / 'replay.jsonl').read_bytes()
        replay_ram = (work / 'replay.dat').read_bytes()
        report['unmodified_replay'] = dict(trace_sha256=digest(replay_trace), final_ram_sha256=digest(replay_ram),
            consumed_input_sha256=digest((work / 'replay-consumed.fa18in').read_bytes()))
        report['unmodified_replay_exact'] = (
            report['unmodified_replay']['trace_sha256'] == report['driver_trace_sha256'] and
            report['unmodified_replay']['final_ram_sha256'] == report['driver_final_ram_sha256'] and
            report['unmodified_replay']['consumed_input_sha256'] == report['consumed_input_sha256'])
        starts = [i for i, r in rows.items() if number(r, 'mode') == 3 and
                  number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1]
        report['mission_three_flight_start'] = starts[0] if starts else None
        report['scope'] = ('Physical-key original recording, independently replayed without the validation pilot. '
                           'Failed route outcomes are preserved. Native whole-flight comparison remains separate.')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        assert report['unmodified_replay_exact'], 'pilot adapter changed original execution beyond its recorded keys'
        print(f'{len(rows)} complete original boundaries and consumed keys reproduce exactly without the pilot; '
              f'mission success {report["mission_success"]}', flush=True)
    for path, expected in hashes.items():
        assert digest((ROOT / path).read_bytes()) == expected, 'sealed media changed'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
