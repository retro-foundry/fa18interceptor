"""Compare normally reached mode-five formation, radar kills and objective.

Original instructions run externally from native before-states. This bounded
objective gate excludes landing and saved success; the full mission has its
own gate. Independent complete-flight parity remains open.
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
END_TICK = 19500


def objective_evidence(exports, formation_length):
    """Require the normally reached formation, radar expiry and objective path."""
    proximity = [item for item in exports if item.get('proximity')]
    assert [item['gate_before'] for item in proximity] == list(range(200, -1, -1))
    assert all(item['gate_after'] == item['gate_before'] - 1 for item in proximity)
    assert [item['body'] for item in proximity] == list(range(proximity[0]['body'], proximity[0]['body'] + 201))
    for item in proximity:
        assert all(abs(a - b) <= item['distance_limit'] for a, b in zip(item['player_fixed'], item['stolen_fixed'])), item
    bodies = [item for item in exports if 'capture' in item]
    hits = [item for item in exports if item.get('weapon_hit')]
    assert len(hits) == 2 and hits[-1]['radar_after'] - hits[0]['radar_before'] == 2, hits
    hit_slots = []
    for hit in hits:
        assert hit['radar_after'] == hit['radar_before'] + 1 and hit['gun_after'] == hit['gun_before'], hit
        targets = [record for record in hit['records'] if record['flags_before'] != record['flags_after']]
        assert len(targets) == 1 and targets[0]['lifetime'] == 15, hit
        hit_slots.append(targets[0]['slot'])
    assert hit_slots == [8, 10], hit_slots
    combat = [item for item in bodies if item['combat_window']]
    expected = {body for hit in hits for body in range(hit['body'], hit['body'] + 20)}
    assert {item['body'] for item in combat} == expected, 'Missing radar/expiry window'
    formation = [item for item in bodies if item['formation_window']]
    expected = {body for start in (proximity[0]['body'], proximity[-1]['body']) for body in range(start, start + formation_length)}
    assert {item['body'] for item in formation} == expected, 'Missing formation window'
    expiries = [item for item in bodies if item['enemy_expiries_before'] != item['enemy_expiries_after']]
    assert [(item['enemy_expiries_before'], item['enemy_expiries_after']) for item in expiries] == [(0, 1), (1, 2)], expiries
    objectives = [item for item in exports if item.get('objective') and not item.get('finished')]
    assert len(objectives) == 1 and objectives[0]['phase'] == 255, objectives
    transitions = [item for item in bodies if item['phase_before'] == 0 and item['phase_after'] == 255]
    assert len(transitions) == 1, transitions
    transition = transitions[0]
    assert transition['after_tick'] == objectives[0]['tick'] and transition['enemy_expiries_after'] == 2, transition
    assert transition['proximity_gate_after'] == -1, transition
    return {'consecutive_formation_window_bodies': len(formation),
            'consecutive_combat_window_bodies': len(combat),
            'native_consecutive_proximity_decrements': len(proximity),
            'radar_hit_increments': 2, 'radar_hit_bodies': hits, 'enemy_expiries': expiries,
            'objective': objectives[0], 'objective_transition': transition}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mission-five-objective')
    args = parser.parse_args()
    work = args.out.resolve()
    env = os.environ.copy()
    env['FA18_MISSION_END_TICK'] = str(END_TICK)
    with CaptureWorkspace(work) as ram:
        prefix, keys = ram / 'frame', work / 'pilot.e9k'
        replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                 str(ram / 'pilot'), str(keys), str(prefix), '5-mission'],
                                cwd=ROOT, capture_output=True, text=True, env=env, timeout=60)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        ends = [item for item in exports if item.get('diagnostic_end')]
        assert replay.returncode == 1 and len(ends) == 1, replay.stderr or replay.stdout
        summary = ends[0]
        assert summary['mode'] == 5 and summary['phase'] == 255 and summary['tick'] == END_TICK, summary
        assert not summary['crash_resets'] and summary['stage'] == 'C10DAE', summary
        assert summary['completions_before'] == summary['completions_after'], summary
        assert not any(item.get('finished') for item in exports), 'Unexpected mission-success result'
        evidence = objective_evidence(exports, 16)
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        assert compare_mission_boundaries(prefix, entries, bodies, work) == 0, 'Unexpected mission config write'
        report = {'scenario': 'normal-key-mode-five-formation-radar-kills-objective',
                  'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
                  **evidence,
                  'summary': summary, 'mission_success_accepted': False,
                  'native_flight_state_seeded': False, 'comparison_masks_changed': False,
                  'reference_scope': 'Original instructions from native before-states; independent complete-flight parity remains open',
                  'input_sha256': hashlib.sha256(keys.read_bytes()).hexdigest(),
                  'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Mode-five objective: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing; two radar kills and objective at tick {evidence['objective']['tick']}")


if __name__ == '__main__':
    main()
