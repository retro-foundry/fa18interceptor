"""Compare a normal-key mode-three objective, landing, taxi and saved result.

Original instructions receive native before-states in a separate process.
The playable runtime receives only ADF assets and keyboard input.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

from capture_workspace import CaptureWorkspace
from mission_source_comparison import compare_mission_boundaries

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mission-success-check')
    args = parser.parse_args()
    work = args.out.resolve()
    with CaptureWorkspace(work) as ram:
        prefix, pilot = ram / 'frame', ram / 'pilot'
        keys = work / 'pilot.e9k'
        replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                 str(pilot), str(keys), str(prefix)],
                                cwd=ROOT, capture_output=True, text=True, timeout=60)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        assert replay.returncode == 0, replay.stderr or replay.stdout
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        summaries = [item for item in exports if item.get('finished')]
        assert len(summaries) == 1, summaries
        summary = summaries[0]
        assert summary['airborne'] and summary['landed'] and summary['objective'] and summary['reloaded'], summary
        assert summary['mode'] == 3 and summary['phase'] == 252 and not summary['crash_resets'], summary
        assert summary['completions_after'] == summary['completions_before'] + 1, summary
        assert summary['grade_after'] == min(summary['grade_before'] + 1, 3), summary
        assert summary['region'] & 4 and not summary['speed'], summary
        landings = [item for item in bodies if item['touchdown']]
        assert len(landings) == 1, landings
        window = [item for item in bodies if item['landing_window']]
        assert len(window) == 96 and [item['body'] for item in window] == list(range(landings[0]['body'], landings[0]['body'] + 96))
        confirmations = [item for item in bodies if item['confirmation_before'] != item['confirmation_after']]
        assert confirmations and any(item['confirmation_after'] == 255 and item['selected'] >= 0 and
                                     item['view_record'] == 0 for item in confirmations), confirmations
        objectives = [item for item in bodies if item['phase_before'] == 0 and item['phase_after'] == 255]
        assert len(objectives) == 1, objectives
        finishes = [item for item in bodies if item['phase_after'] == 252 and item['phase_before'] != 252]
        assert len(finishes) == 1 and finishes[0]['ready'], finishes
        results = [item for item in entries if item['stage'] == 'C110A4' and
                   item['phase_before'] == 252 and item['completions'] == summary['completions_after']]
        assert len(results) == 1, results
        saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78 and int.from_bytes(saved[56:58], 'big') == summary['completions_after']
        assert saved[6] == 3 and saved[7] == summary['grade_before'] and saved[21] == summary['grade_after']
        writes = compare_mission_boundaries(prefix, entries, bodies, work)
        assert writes == 1, f'Expected the actual mission config write; observed {writes}'
        report = {'scenario': 'normal-key-mode-three-objective-landing-taxi-result-reload',
                  'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
                  'consecutive_landing_bodies': len(window), 'original_config_writes': writes,
                  'summary': summary, 'objective': objectives[0], 'touchdown': landings[0],
                  'finish': finishes[0], 'result': results[0],
                  'native_flight_state_seeded': False,
                  'reference_scope': 'Original instructions from native before-states; independent full-flight parity remains open',
                  'input_sha256': hashlib.sha256(keys.read_bytes()).hexdigest(),
                  'saved_config_sha256': hashlib.sha256(saved).hexdigest(),
                  'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Mode three mission: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing, including all {len(window)} landing bodies and the actual config write")


if __name__ == '__main__':
    main()
