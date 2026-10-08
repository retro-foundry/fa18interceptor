"""Replay an earned tour in the playable runner across cold save/load boundaries.

The save directory starts empty. Only the runner writes pilot config; no
qualification, grade, eligibility or flight state is supplied by the checker.
Original sampled comparisons are separate per-mission gates, not an
independent original whole-tour replay.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile

from region_pilot_fixture import load_region_pilot
from tour_pilot_fixture import load_tour_pilot, tour_input, load_tour_result

ROOT = Path(__file__).resolve().parents[2]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/new-pilot-tour')
    parser.add_argument('--timeout', type=float, default=90, help='Deadline per playable stage, seconds')
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    fixtures = ROOT / 'tools/native/fixtures'
    region = json.loads((fixtures / 'region-flight-pilot.json').read_text())
    stolen = json.loads((fixtures / 'stolen-mission-pilot.json').read_text())
    later = json.loads((fixtures / 'new-pilot-tour-availability.json').read_text())
    final, result = load_tour_result()
    adf = ROOT / 'local/media/fa18.adf'
    assert sha(adf.read_bytes()) == region['adf_sha256'] == result['adf_sha256']
    stages = []
    with tempfile.TemporaryDirectory(prefix='pilot-', dir=work) as temporary:
        pilot = Path(temporary)
        config = pilot / 'config'
        base = [str(args.runner.resolve()), '--adf', str(adf), '--save-dir', str(pilot), '--headless']

        def run(name, frames, replay, *, expected=None, expected_sha=None, menu=False, extra=()):
            before = sha(config.read_bytes()) if config.exists() else None
            completed = subprocess.run([*base, '--frames', str(frames), '--replay', str(replay), *extra],
                cwd=ROOT, capture_output=True, text=True, timeout=args.timeout)
            (work / f'{name}.log').write_text(completed.stdout + completed.stderr)
            assert completed.returncode == 0, f'{name}: {completed.stderr or completed.stdout}'
            state, = [json.loads(line) for line in completed.stdout.splitlines() if line.startswith('{')]
            assert not state['cpu_emulation'] and not state['chipset_emulation'] and not state['postflight_resets'], state
            assert not state['host_replay_pending'] and not state['input_queued'], state
            assert state['host_replay_events'] == len(replay.read_text().splitlines()) - 1, state
            if menu:
                assert state['screen'] == 'menu' and state['stage'] == 'C0FCB4', state
            saved = config.read_bytes()
            assert len(saved) == 78
            if expected is not None:
                assert saved == expected, f'{name}: earned save differs'
            if expected_sha is not None:
                assert sha(saved) == expected_sha, f'{name}: earned save hash differs'
            stages.append({'name': name, 'initial_config_sha256': before,
                'saved_config_sha256': sha(saved), 'saved_config_hex': saved.hex(),
                'input_sha256_lf': sha(replay.read_bytes().replace(b'\r\n', b'\n')), 'canonical': state})
            print(f'{name}: earned save {sha(saved)}, {state["screen"]}', flush=True)
            return state

        assert not config.exists()
        run('enlist', 9000, fixtures / 'region-pilot-enlist.e9k',
            expected_sha=region['initial_config_sha256'], menu=True)
        acknowledge = pilot / 'acknowledge.e9k'
        acknowledge.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        qualified = run('qualification', 45000, acknowledge,
            expected_sha=region['qualification_config_sha256'],
            extra=('--input', str(fixtures / 'carrier_qualification_game_input.fa18in'),
                   '--iterations', str(region['qualification_iterations'])))
        assert qualified['replay_iterations'] == region['qualification_iterations'] and qualified['stage'] == 'C10DAE'
        stages[-1]['game_input_sha256_lf'] = sha((fixtures / 'carrier_qualification_game_input.fa18in').read_bytes().replace(b'\r\n', b'\n'))
        assert stages[-1]['game_input_sha256_lf'] == region['input_sha256_lf']['tools/native/fixtures/carrier_qualification_game_input.fa18in']
        run('mission-3', region['mission_three_menu_tick'], fixtures / 'region-pilot-mission-three.e9k',
            expected=load_region_pilot(), menu=True)
        run('mission-4', stolen['mission_four_menu_tick'], fixtures / 'mission-four-sequence.e9k',
            expected=load_tour_pilot(5), menu=True)
        for mode in (5, 6, 7):
            next_stage = later['missions'][str(mode + 1)]
            run(f'mission-{mode}', next_stage['previous_menu_tick'], tour_input(mode),
                expected=load_tour_pilot(mode + 1), menu=True)
        run('mission-8', result['menu_tick'], ROOT / result['input'], expected=final, menu=True)
        wrap = run('next-mission-wrap', result['wrap_tick'], ROOT / result['wrap_input'], expected=final)
        assert wrap['mode'] == 3 and wrap['screen'] == 'mode-intro' and wrap['stage'] == result['wrap_stage'], wrap
        assert final[:4] == b'\0\1\0\0' and final[21:27] == b'\1' * 6 and int.from_bytes(final[56:58], 'big') == 6
    report = {'pilot_newly_enlisted': True, 'full_tour_earned': True,
        'native_flight_state_seeded': False, 'pilot_config_written_by_checker': False,
        'save_load_boundaries': 'Cold runner starts sharing the actual preceding saved log',
        'reference_scope': 'Playable earned tour; original sampled comparisons remain separate and independent original whole-flight parity remains open',
        'runner_sha256': sha(args.runner.read_bytes()), 'adf_sha256': sha(adf.read_bytes()), 'stages': stages}
    (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print('New pilot earns qualification, all six missions and the cold Next Mission wrap', flush=True)


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
