"""Compare normal-key mission combat diagnostics and natural reset.

This gate covers the failed flight's reset, not a successful mission.
The playable runtime receives only ADF assets and ordinary keyboard input.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

from capture_workspace import CaptureWorkspace
from mission_source_comparison import compare_mission_boundaries

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mission-five-combat-reset')
    parser.add_argument('--mode', type=int, choices=(4, 5), default=5)
    args = parser.parse_args()
    work = args.out.resolve()
    with CaptureWorkspace(work) as ram:
        prefix, keys = ram / 'frame', work / 'pilot.e9k'
        env = os.environ.copy()
        env['FA18_MISSION_END_TICK'] = '30000'
        replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                 str(ram / 'pilot'), str(keys), str(prefix), '4-mission' if args.mode == 4 else '5'],
                                cwd=ROOT, capture_output=True, text=True, env=env, timeout=60)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        ends = [item for item in exports if item.get('diagnostic_end')]
        assert replay.returncode == 1 and len(ends) == 1, replay.stderr or replay.stdout
        summary = ends[0]
        assert summary['mode'] == args.mode and summary['crash_resets'] == 1 and summary['stage'] == 'C11830', summary
        assert summary['completions_before'] == summary['completions_after'], summary
        assert not any(item.get('finished') for item in exports), 'Unexpected mission-success result'
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        hits = [item for item in exports if item.get('weapon_hit')]
        if args.mode == 5:
            assert hits and hits[-1]['gun_after'] - hits[0]['gun_before'] == 7, hits
            assert all(item['radar_after'] == item['radar_before'] for item in hits), hits
        else:
            assert not hits, hits
            fire_ticks = [int(line.split()[1]) for line in keys.read_text().splitlines()
                          if line.split()[3:] == ['32', '0', '0', '1']]
            assert len(fire_ticks) == 43 and fire_ticks[:4] == [1800, 12617, 13123, 13629], fire_ticks
            assert not any(item.get('objective') for item in exports), 'Unexpected mission objective'
        windows = [item for item in bodies if item['combat_window']]
        expected = {body for hit in hits for body in range(hit['body'], hit['body'] + 20)}
        assert {item['body'] for item in windows} == expected, 'Missing combat/expiry boundary'
        resets = [item for item in bodies if item['stage'] == 'C11830']
        assert len(resets) == 1 and resets[0]['after_tick'] == summary['tick'], resets
        # Original builds share objects. Complete each component serially.
        for name in (('records',) if args.mode == 4 else ('candidate_reference', 'depth_sort')):
            oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
            subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                            '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
            component = subprocess.run([str(oracle), f"{prefix}.body.{resets[0]['capture']}.before.dat"],
                                       cwd=ROOT, capture_output=True, text=True, timeout=15)
            (work / f'{name}.log').write_text(component.stdout + component.stderr)
            assert component.returncode == 0, component.stderr or component.stdout
            if args.mode == 4:
                assert '576 complete C2DEE0 matrix transforms match returned angles/divisor/cursor' in component.stdout
        assert compare_mission_boundaries(prefix, entries, bodies, work) == 0, 'Unexpected result config write'
        expiries = [item for item in bodies if item['enemy_expiries_before'] != item['enemy_expiries_after']]
        assert [(item['enemy_expiries_before'], item['enemy_expiries_after']) for item in expiries] == ([(0, 1), (1, 2)] if args.mode == 5 else []), expiries
        report = {'scenario': 'normal-key-mode-five-gun-combat-expiry-reset',
                  'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
                  'consecutive_combat_window_bodies': len(windows),
                  'gun_hit_increments': 7, 'gun_hit_bodies': hits, 'enemy_expiries': expiries,
                  'candidate_reference_cases': 64,
                  'depth_sort_cases': 128, 'distance_factor_cases': 256,
                  'reset': resets[0], 'summary': summary, 'mission_success_accepted': False,
                  'native_flight_state_seeded': False, 'comparison_masks_changed': False,
                  'reference_scope': 'Original instructions from native before-states; independent full-flight parity remains open',
                  'input_sha256': hashlib.sha256(keys.read_bytes()).hexdigest(),
                  'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        if args.mode == 4:
            for field in ('gun_hit_increments', 'candidate_reference_cases', 'depth_sort_cases', 'distance_factor_cases'):
                report.pop(field)
            continuations = [item for item in bodies if item['stage'] == 'C11788']
            assert len(continuations) == 1 and continuations[0]['before_tick'] == 21983, continuations
            report.update(scenario='normal-key-mode-four-missile-attempts-crash-reset',
                          matrix_transform_cursor_cases=576, fire_presses=len(fire_ticks) - 1,
                          first_fire_ticks=fire_ticks[1:4],
                          crash_continuation=continuations[0])
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Mode-{args.mode} combat/reset: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing, including all {len(windows)} combat-window bodies and the reset")


if __name__ == '__main__':
    main()
