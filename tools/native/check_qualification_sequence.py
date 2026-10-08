"""Compare the actual carrier landing, success and restart with original bytes.

Reuses consumed-key evidence. Native flight never receives reference RAM.
This is a connected sequence check, not independent whole-flight parity.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

from capture_workspace import CaptureWorkspace, retain_failure

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_qualification_sequence_test.exe')
    parser.add_argument('--game-input', type=Path, default=ROOT / 'tools/native/fixtures/carrier_qualification_game_input.fa18in')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/qualification-sequence')
    parser.add_argument('--new-pilot', action='store_true', help='Reopen a validation-only saved pilot with qualification word zero')
    args = parser.parse_args()
    assert args.game_input.read_text().splitlines()[0] == 'FA18_GAME_INPUT_V1'
    work = args.out.resolve()
    with CaptureWorkspace(work) as ram:
        prefix = ram / 'frame'
        pilot = ram / 'pilot'
        command = [str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                   str(pilot), str(args.game_input.resolve()), str(prefix)]
        if args.new_pilot:
            command.append('new-pilot')
        replay = subprocess.run(command,
                                cwd=ROOT, capture_output=True, text=True, timeout=35)
        (work / 'native.log').write_text(replay.stdout + replay.stderr)
        exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
        (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        assert replay.returncode == 0, replay.stderr or replay.stdout
        entries = [item for item in exports if 'entry' in item]
        bodies = [item for item in exports if 'capture' in item]
        summary = [item for item in exports if item.get('sequence')]
        assert len(summary) == 1 and summary[0]['iterations'] == 8038 and summary[0]['events'] == 554, summary
        assert summary[0]['airborne'] and summary[0]['landed'] and summary[0]['success'] and summary[0]['restarted'], summary
        assert summary[0]['reloaded'], summary
        if args.new_pilot:
            assert summary[0]['initial_qualification'] == 0, summary
        assert summary[0]['mode'] == 9 and summary[0]['stage'] == 'C10DAE', summary
        assert not summary[0]['queued'] and not summary[0]['crash_resets'], summary
        landings = [item for item in bodies if item['touchdown']]
        assert len(landings) == 1, landings
        window = [item for item in bodies if item['landing_window']]
        assert len(window) == 96 and [item['body'] for item in window] == list(range(landings[0]['body'], landings[0]['body'] + 96))
        assert any(item['phase_before'] == 0 and item['phase_after'] == 255 for item in window), window
        # C11078 is called inside the flight callback and publishes C110A4;
        # it is not itself observed as the next update's callback here.
        required = {'C110A4', 'C0F946', 'C0F974', 'C0F992', 'C10DAE'}
        assert required <= {item['stage'] for item in entries}, entries
        results = [item for item in entries if item['stage'] == 'C110A4' and
                   item['phase_before'] == 255 and item['phase_after'] == 239]
        assert len(results) == 1 and results[0]['qualification'] == 1, results
        saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78 and saved[:2] == b'\0\1', 'Successful qualification was not saved'
        writes = 0
        for name, cases in (('mode_entry', entries), ('frame_body', bodies)):
            oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
            subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                            '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
            with (work / f'{name}.log').open('w') as log:
                for item in cases:
                    kind, index = ('entry', item['entry']) if name == 'mode_entry' else ('body', item['capture'])
                    capture = f'{prefix}.{kind}.{index}'
                    values = [str(item['tick']), *map(str, item['keys'])] if name == 'mode_entry' else [
                        str(item['before_tick']), str(item['after_tick']), str(item['saved_tick']), capture + '.source.dat']
                    if name == 'frame_body' and item['owner_exit']:
                        values.append('owner-exit')
                    check = subprocess.run([str(oracle), capture + '.before.dat', capture + '.after.dat', *values],
                                           cwd=ROOT, capture_output=True, text=True, timeout=15)
                    log.write(check.stdout + check.stderr);log.flush()
                    if check.returncode:
                        retain_failure(capture, work)
                        raise RuntimeError(f"{kind} {index}, update {item['iteration']}: {check.stderr or check.stdout}")
                    for line in check.stdout.splitlines():
                        if line.startswith('Complete original file owners reached DOS Write '):
                            writes += int(line.split('Write ')[1].split()[0])
                    for suffix in ('before', 'after', 'source'):
                        Path(capture + f'.{suffix}.dat').unlink(missing_ok=True)
        assert writes == 1, f'Expected the real qualification config write; observed {writes}'
        report = {'scenario': 'recorded-input-carrier-landing-success-restart',
                  'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
                  'consecutive_landing_bodies': len(window), 'original_config_writes': writes,
                  'summary': summary[0], 'touchdown': landings[0],
                  'result': results[0],
                  'native_flight_state_seeded': False,
                  'saved_pilot_fixture': 'Qualification word zero, saved and reopened before replay' if args.new_pilot else 'None; original ADF log',
                  'reference_scope': 'Original instructions from native before-states; independent full-flight parity remains open',
                  'input_sha256': hashlib.sha256(args.game_input.read_bytes()).hexdigest(),
                  'test_sha256': hashlib.sha256(args.test.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"Carrier qualification{' with new pilot' if args.new_pilot else ''}: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing, including all {len(window)} consecutive landing bodies and the actual config write")


if __name__ == '__main__':
    main()
