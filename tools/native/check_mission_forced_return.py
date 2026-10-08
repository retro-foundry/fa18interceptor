"""Compare normally reached mode-five proximity and paired aircraft restoration.

Original instructions run externally from native before-states. The runtime
receives only the ADF and keyboard events. Complete mission success stays open.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

from capture_workspace import CaptureWorkspace
from mission_source_comparison import compare_mission_boundaries

ROOT = Path(__file__).resolve().parents[2]
END_TICK = 18130


def restored_views(data):
    """Read C0A12E's actual table and resulting views from exported native RAM."""
    offset = lambda address: 0x80000 + address - 0xc00000
    signed = lambda address, size: int.from_bytes(data[offset(address):offset(address) + size], 'big', signed=True)
    views = []
    for slot in (4, 6):
        record = 0xc46184 + 512 * slot
        kind = data[offset(record + 0x3a)]
        table = 0xc295e0 + signed(0xc295e0 + 2 * kind, 2)
        expected = [signed(table + 2 * i, 2) for i in range(5)]
        actual = [signed(record + 0x2c + 2 * i, 2) for i in range(4)] + [signed(record + 0x34, 4)]
        views.append({'slot': slot, 'kind': kind, 'expected': expected, 'actual': actual})
    return views


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mission-five-forced-return')
    args = parser.parse_args()
    work = args.out.resolve()
    env = os.environ.copy()
    env['FA18_MISSION_END_TICK'] = str(END_TICK)
    with CaptureWorkspace(work) as ram:
        prefix, keys = ram / 'frame', work / 'pilot.e9k'
        replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                 str(ram / 'pilot'), str(keys), str(prefix), '5-formation'],
                                cwd=ROOT, capture_output=True, text=True, env=env, timeout=60)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        ends = [item for item in exports if item.get('diagnostic_end')]
        assert replay.returncode == 1 and len(ends) == 1, replay.stderr or replay.stdout
        summary = ends[0]
        assert summary['mode'] == 5 and summary['phase'] == 0 and summary['tick'] == END_TICK, summary
        assert not summary['crash_resets'] and summary['stage'] == 'C10DAE', summary
        assert summary['completions_before'] == summary['completions_after'], summary
        assert not any(item.get('finished') or item.get('objective') for item in exports), 'Unexpected mission-success result'
        proximity = [item for item in exports if item.get('proximity')]
        assert [item['gate_before'] for item in proximity] == list(range(200, -1, -1)), 'Incomplete proximity countdown'
        assert all(item['gate_after'] == item['gate_before'] - 1 for item in proximity)
        assert [item['body'] for item in proximity] == list(range(proximity[0]['body'], proximity[0]['body'] + 201))
        for item in proximity:
            assert all(abs(a - b) <= item['distance_limit'] for a, b in zip(item['player_fixed'], item['stolen_fixed'])), item
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        windows = [item for item in bodies if item['formation_window']]
        expected = {body for start in (proximity[0]['body'], proximity[-1]['body']) for body in range(start, start + 32)}
        assert len(windows) == 64 and {item['body'] for item in windows} == expected, 'Missing proximity/restoration window'
        native_views = {}
        for item in bodies:
            if item['proximity_gate_before'] >= 0:
                continue
            data = Path(f"{prefix}.body.{item['capture']}.after.dat").read_bytes()
            native_views[item['body']] = restored_views(data)
        assert compare_mission_boundaries(prefix, entries, bodies, work) == 0, 'Unexpected mission config write'
        chunks = (work / 'frame_body.log').read_text().split('Frame input carry: ')[1:]
        assert len(chunks) == len(bodies), 'Missing original execution report'
        restorations = []
        for item, chunk in zip(bodies, chunks):
            match = re.search(r'Mode-five restoration: (\d+) first-record calls, (\d+) second-record calls', chunk)
            if not match:
                continue
            first, second = map(int, match.groups())
            assert first == second == 1 and item['proximity_gate_before'] < 0, item
            views = native_views[item['body']]
            assert all(view['actual'] == view['expected'] for view in views), views
            restorations.append({'body': item['body'], 'before_tick': item['before_tick'],
                                 'first_calls': first, 'second_calls': second, 'views': views})
        assert restorations and restorations[0]['body'] == proximity[-1]['body'] + 1, 'Original paired restoration did not execute'
        report = {'scenario': 'normal-key-mode-five-proximity-paired-restoration',
                  'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
                  'consecutive_proximity_restoration_window_bodies': len(windows),
                  'native_consecutive_proximity_decrements': len(proximity),
                  'original_first_restoration_calls': sum(item['first_calls'] for item in restorations),
                  'original_second_restoration_calls': sum(item['second_calls'] for item in restorations),
                  'restorations': restorations, 'summary': summary, 'mission_success_accepted': False,
                  'native_flight_state_seeded': False, 'comparison_masks_changed': False,
                  'reference_scope': 'Original instructions from native before-states; independent complete-flight parity remains open',
                  'input_sha256': hashlib.sha256(keys.read_bytes()).hexdigest(),
                  'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Mode-five forced return: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing; 201 native countdown scans, {len(windows)} consecutive window bodies, {len(restorations)} original paired restorations")


if __name__ == '__main__':
    main()
