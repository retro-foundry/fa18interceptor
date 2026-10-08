"""Compare three final-mission patrol destructions, below the original quota.

This is an incomplete flight diagnostic. It uses the established availability-
only saved-pilot fixture, ordinary keyboard events and original boundary oracles.
It does not accept mission success, saved progression or an independent flight.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

from capture_workspace import CaptureWorkspace
from mission_source_comparison import compare_mission_boundaries

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/final-patrol-check')
    parser.add_argument('--timeout', type=float, default=60)
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    work = args.out.resolve()
    fixture = json.loads((ROOT / 'tools/native/fixtures/final-mission-eligible-pilot.json').read_text())
    initial = bytes.fromhex(fixture['config_hex'])
    assert len(initial) == 78 and initial[25] == 1
    assert hashlib.sha256(initial).hexdigest() == fixture['config_sha256']
    adf = ROOT / 'local/media/fa18.adf'
    assert hashlib.sha256(adf.read_bytes()).hexdigest() == fixture['adf_sha256']
    assert not fixture['mission_availability_earned'] and not fixture['native_flight_state_seeded']
    with CaptureWorkspace(work) as ram:
        pilot, prefix = ram / 'pilot', ram / 'frame'
        pilot.mkdir()
        (pilot / 'config').write_bytes(initial)
        keys = work / 'pilot.e9k'
        env = dict(os.environ, FA18_MISSION_END_TICK='21000')
        run = subprocess.run([str(args.test.resolve()), str(adf), str(pilot), str(keys), str(prefix), '8-success'],
                             cwd=ROOT, env=env, capture_output=True, text=True, timeout=args.timeout)
        (work / 'native.log').write_text(run.stdout + run.stderr)
        exports = [json.loads(line) for line in run.stdout.splitlines() if line.startswith('{')]
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        end, = [item for item in exports if item.get('diagnostic_end')]
        status, = [item for item in exports if item.get('final_mission_status')]
        hits = [item for item in exports if item.get('weapon_hit')]
        counters = [item for item in exports if item.get('final_mission_counter') and item['expiries_before'] != item['expiries_after']]
        # The native success fixture must reject this bounded, incomplete flight.
        assert run.returncode == 1 and 'Mission incomplete:' in run.stderr, run.stderr
        assert end['mode'] == 8 and end['phase'] == 0 and end['tick'] == 21000 and not end['crash_resets'], end
        assert end['completions_before'] == end['completions_after'] == 3, end
        assert not status['objective'] and status['admitted'] == 4 and status['expiries'] == 3, status
        assert status['record_14_kind'] == 0x20 and status['record_14_flags'] & 0x40, status
        assert len(hits) == len(counters) == 3, (hits, counters)
        windows = []
        for index, (hit, counter) in enumerate(zip(hits, counters)):
            assert counter['admitted_before'] == counter['admitted_after'] == 4, counter
            assert counter['expiries_before'] == index and counter['expiries_after'] == index + 1, counter
            assert counter['phase_before'] == counter['phase_after'] == 0, counter
            victims = [record for record in hit['records'] if not record['flags_before'] & 0x400 and record['flags_after'] & 0x400]
            victim, = victims
            assert victim['lifetime'] == 15, victim
            expired, = [record for record in counter['records'] if record['slot'] == victim['slot']]
            assert expired['kind'] & 0xf0 == 0x10 and expired['lifetime_before'] == 1 and expired['lifetime_after'] == 0, expired
            assert expired['flags_before'] & 0x400 and not expired['flags_after'] & 0x400, expired
            assert not expired['flags_before'] & 8 and not expired['contact'] & 0x80, expired
            assert counter['body'] == hit['body'] + 15, counter
            window = [item for item in bodies if hit['body'] <= item['body'] < hit['body'] + 20]
            assert len(window) == 20 and [item['body'] for item in window] == list(range(hit['body'], hit['body'] + 20)), window
            windows.append({'slot': victim['slot'], 'hit_body': hit['body'], 'expiry_body': counter['body'], 'compared_bodies': len(window)})
        assert [(hit['radar_after'] - hit['radar_before'], hit['infrared_after'] - hit['infrared_before']) for hit in hits] == [(1, 0), (1, 0), (0, 1)], hits
        assert (pilot / 'config').read_bytes() == initial
        writes = compare_mission_boundaries(prefix, entries, bodies, work)
        assert writes == 0, writes
        canonical_pilot = ram / 'canonical-pilot'
        canonical_pilot.mkdir()
        (canonical_pilot / 'config').write_bytes(initial)
        final_data = ram / 'canonical-final.dat'
        playable = subprocess.run([str(args.runner.resolve()), '--adf', str(adf), '--save-dir', str(canonical_pilot),
            '--headless', '--frames', str(end['tick']), '--replay', str(keys), '--data-out', str(final_data)],
            cwd=ROOT, capture_output=True, text=True, timeout=args.timeout)
        (work / 'canonical.log').write_text(playable.stdout + playable.stderr)
        assert playable.returncode == 0, playable.stderr
        state, = [json.loads(line) for line in playable.stdout.splitlines() if line.startswith('{')]
        assert state['mode'] == 8 and state['stage'] == end['stage'] and not state['postflight_resets'], state
        assert not state['cpu_emulation'] and not state['chipset_emulation'], state
        assert state['host_replay_events'] == len(keys.read_text().splitlines()) - 1 and not state['host_replay_pending'], state
        assert not state['input_queued'] and (canonical_pilot / 'config').read_bytes() == initial, state
        data = final_data.read_bytes()
        assert len(data) == 0x100000

        def value(address, size=1):
            offset = address if address < 0x80000 else 0x80000 + address - 0xc00000
            return int.from_bytes(data[offset:offset + size], 'big')

        # Original globals.h addresses. This observes the actual playable
        # runner's counters/hits, independently of its host-input summary.
        log = value(0xc1ab74, 4)
        canonical_counters = {'admitted': value(0xc458aa), 'expiries': value(0xc458ab),
            'phase': value(0xc45798), 'completions': value(log + 56, 2),
            'radar_hits': value(log + 68, 2), 'infrared_hits': value(log + 64, 2),
            'record_14_kind': value(0xc46184 + 512 * 14 + 98),
            'record_14_flags': value(0xc46184 + 512 * 14, 2)}
        assert canonical_counters == {'admitted': 4, 'expiries': 3, 'phase': 0, 'completions': 3,
            'radar_hits': hits[-1]['radar_after'], 'infrared_hits': hits[-1]['infrared_after'],
            'record_14_kind': status['record_14_kind'], 'record_14_flags': status['record_14_flags']}, canonical_counters
        final_data.unlink()
        report = {'scenario': 'final-mission-three-patrol-destructions-below-quota',
            'mission_success_accepted': False, 'mission_availability_earned': False,
            'eligibility_fixture': 'tools/native/fixtures/final-mission-eligible-pilot.json',
            'native_flight_state_seeded': False,
            'reference_scope': 'Original instructions from native before-states; independent complete-flight parity remains open',
            'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
            'counter_windows': windows, 'counter_events': counters, 'final': end, 'status': status,
            'source_config_writes': writes, 'canonical': state, 'canonical_counters': canonical_counters,
            'initial_config_sha256': fixture['config_sha256'], 'adf_sha256': fixture['adf_sha256'],
            'input_sha256_lf': hashlib.sha256(keys.read_text().encode()).hexdigest(),
            'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
            'runner_sha256': hashlib.sha256(args.runner.read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Final patrol diagnostic: {len(entries)} intervals/{len(bodies)} bodies match original; three aircraft expiries, quota four, no completion/save")


if __name__ == '__main__':
    main()
