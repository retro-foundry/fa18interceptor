"""Compare normal-key mode-five gun damage, expiry windows and natural reset.

This gate covers the failed flight's reset, not a successful mode-five mission.
The playable runtime receives only ADF assets and ordinary keyboard input.
"""
import argparse
import hashlib
import json
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
    args = parser.parse_args()
    work = args.out.resolve()
    with CaptureWorkspace(work) as ram:
        prefix, keys = ram / 'frame', work / 'pilot.e9k'
        replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                 str(ram / 'pilot'), str(keys), str(prefix), '5'],
                                cwd=ROOT, capture_output=True, text=True, timeout=60)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        ends = [item for item in exports if item.get('diagnostic_end')]
        assert replay.returncode == 1 and len(ends) == 1, replay.stderr or replay.stdout
        summary = ends[0]
        assert summary['mode'] == 5 and summary['crash_resets'] == 1 and summary['stage'] == 'C11830', summary
        assert summary['completions_before'] == summary['completions_after'], summary
        assert not any(item.get('finished') for item in exports), 'Unexpected mission-success result'
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        hits = [item for item in exports if item.get('weapon_hit')]
        assert hits and hits[-1]['gun_after'] - hits[0]['gun_before'] == 7, hits
        assert all(item['radar_after'] == item['radar_before'] for item in hits), hits
        windows = [item for item in bodies if item['combat_window']]
        expected = {body for hit in hits for body in range(hit['body'], hit['body'] + 20)}
        assert {item['body'] for item in windows} == expected, 'Missing combat/expiry boundary'
        resets = [item for item in bodies if item['stage'] == 'C11830']
        assert len(resets) == 1 and resets[0]['after_tick'] == summary['tick'], resets
        # Original builds share objects. Complete each component serially.
        for name in ('candidate_reference', 'depth_sort'):
            oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
            subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                            '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
            component = subprocess.run([str(oracle), f"{prefix}.body.{resets[0]['capture']}.before.dat"],
                                       cwd=ROOT, capture_output=True, text=True, timeout=15)
            (work / f'{name}.log').write_text(component.stdout + component.stderr)
            assert component.returncode == 0, component.stderr or component.stdout
        assert compare_mission_boundaries(prefix, entries, bodies, work) == 0, 'Unexpected result config write'
        expiries = [item for item in bodies if item['enemy_expiries_before'] != item['enemy_expiries_after']]
        assert [(item['enemy_expiries_before'], item['enemy_expiries_after']) for item in expiries] == [(0, 1), (1, 2)], expiries
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
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Mode-five combat/reset: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing, including all {len(windows)} combat-window bodies and the reset")


if __name__ == '__main__':
    main()
