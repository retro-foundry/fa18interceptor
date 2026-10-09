"""Preserve native PCM/state/saves while enforcing the gameplay heap guard.

The comparison is against accepted native output, not original Amiga filtering.
Raw passing WAV/RAM/pixel files are temporary; reports and hashes are retained.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

from tour_pilot_fixture import load_tour_pilot, load_tour_result

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    with path.open('rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--baseline', type=Path)
    source.add_argument('--reference', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--visible', action='store_true', help='also present the 6500-frame Free Flight with real audio')
    parser.add_argument('--pcm-change', action='store_true',
                        help='Assess intentional PCM processing change: require all RAM/pixels/saves and non-PCM counters unchanged; retain new PCM hashes')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    reference = json.loads(args.reference.read_text()) if args.reference else None
    final_input = ROOT / load_tour_result()[1]['input']
    report = {'runner_sha256': sha(args.runner), 'baseline_sha256': sha(args.baseline) if args.baseline else reference['runner_sha256'],
              'scope': 'Native output preservation; project heap guard and fixed SDL/audio storage; OS/driver heaps unobserved', 'cases': {}}
    if args.pcm_change:
        report['scope'] = 'Intentional original-derived PCM processing change; unchanged native RAM/pixels/saves/non-PCM counters; zero gameplay heap violations'
        report['pcm_change'] = {}
    with tempfile.TemporaryDirectory(prefix='preallocation-', dir=args.out) as temporary:
        work = Path(temporary)
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\nF 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        flight = work / 'flight.e9k'
        flight.write_text('E9K_INPUT_V1\n' + ''.join(
            f'F {frame} K {key} 0 0 1\nF {frame+2} K {key} 0 0 0\n'
            for frame, key in ((1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49))))

        def run(runner, name, frames, replay, guarded, visible=False, pilot=None):
            folder = work / name
            folder.mkdir()
            if pilot:
                (folder / 'config').write_bytes(pilot)
            command = [str(runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                       '--save-dir', str(folder), '--frames', str(frames), '--replay', str(replay),
                       '--wav', str(folder / 'audio.wav'), '--data-out', str(folder / 'data.dat'),
                       '--ppm', str(folder / 'pixels.ppm')]
            if not visible:
                command += ['--headless']
            if guarded:
                command += ['--memory-report', str(folder / 'memory.json')]
            if visible:
                command += ['--recorded-input-only', '--frame-times', str(folder / 'times.csv')]
            result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=180 if visible else 90)
            (args.out / f'{name}.log').write_text(result.stdout + result.stderr)
            assert result.returncode == 0, result.stderr
            values = {'stats': json.loads(result.stdout), **{suffix: sha(folder / filename) for suffix, filename in
                (('wav_sha256', 'audio.wav'), ('data_sha256', 'data.dat'), ('ppm_sha256', 'pixels.ppm'))},
                'save_sha256': sha(folder / 'config') if (folder / 'config').exists() else None}
            if guarded:
                memory = json.loads((folder / 'memory.json').read_text())
                assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == 0, memory
                values['memory'] = memory
            if visible:
                import csv
                with (folder / 'times.csv').open(newline='') as file:
                    rows = list(csv.DictReader(file))
                assert len(rows) == frames and all(row['presented'] == '1' for row in rows)
                assert values['stats']['audio_device']
                (args.out / 'visible-times.csv').write_bytes((folder / 'times.csv').read_bytes())
            return values

        for name, frames, replay, pilot in (('intro', 3100, intro, None), ('free-flight', 6500, flight, None),
                                          ('final-combat', 24200, final_input, load_tour_pilot(8))):
            before = run(args.baseline, 'baseline-' + name, frames, replay, False, pilot=pilot) if args.baseline else {
                key: value for key, value in reference['cases'][name].items() if key != 'memory'}
            after = run(args.runner, name, frames, replay, True, pilot=pilot)
            comparable = {key: value for key, value in after.items() if key != 'memory'}
            if args.pcm_change:
                def state_only(value):
                    return {key: ({k:v for k,v in item.items() if k != 'nonzero_sample_frames'} if key == 'stats' else item)
                            for key,item in value.items() if key != 'wav_sha256'}
                assert state_only(before) == state_only(comparable), name
                assert before['wav_sha256'] != after['wav_sha256'], 'Expected an intentional PCM processing change'
                report['pcm_change'][name] = {'before_wav_sha256': before['wav_sha256'],
                    'before_nonzero_sample_frames': before['stats']['nonzero_sample_frames'],
                    'game_state_and_non_pcm_counters_unchanged': True}
            else:
                assert before == comparable, name
            report['cases'][name] = after
            print(f'{name}: '+ ('processed PCM changed; complete RAM/pixels/save/non-PCM counters preserved' if args.pcm_change else 'complete WAV, RAM, pixels, save and counters preserved') + '; zero gameplay heap violations', flush=True)
        if args.visible:
            visible = run(args.runner, 'visible-free-flight', 6500, flight, True, visible=True)
            expected = report['cases']['free-flight']
            assert all(visible[key] == expected[key] for key in ('wav_sha256', 'data_sha256', 'ppm_sha256', 'save_sha256'))
            expected_stats = dict(expected['stats'], audio_device=True)
            assert visible['stats'] == expected_stats
            report['visible'] = visible
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
