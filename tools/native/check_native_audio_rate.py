"""Exercise both host output rates through ordinary native startup and flights.

Compare complete game state and ordered sound requests at equal physical times.
PCM is captured at each declared rate; it is never resampled or aligned.
"""
import argparse
import copy
import gzip
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import wave

from check_native_audio_trace import read_audio_trace
from tour_pilot_fixture import load_tour_pilot, load_tour_result

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    with path.open('rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def same_physical_sound(left, right):
    assert len(left) == len(right), 'Sound event count changed with output rate'
    boundaries = [[row for row in rows if row['kind'] == 'boundary'] for rows in (left, right)]
    assert len(boundaries[0]) == len(boundaries[1])
    for a, b in zip(*boundaries):
        aa, bb = dict(a), dict(b)
        del aa['sample_frames']; del bb['sample_frames']
        aa['channels'] = [dict(c) for c in a['channels']]
        bb['channels'] = [dict(c) for c in b['channels']]
        for x, y in zip(aa['channels'], bb['channels']):
            assert x.pop('phase') * 44100 == y.pop('phase') * 48000, 'Byte phase changed in physical time'
        assert aa == bb, 'Game/voice state changed with output rate'
    events = 0
    for channel in range(4):
        # Channels have separate buffer/voice owners. Requests on different
        # channels can share an enclosing sample at one rate and occupy two
        # adjacent samples at the other. Compare each channel's complete
        # ordinal sequence and its actual enclosing interval, without sorting,
        # searching, dropping or moving any event on that channel.
        per_channel = [[row for row in rows if row.get('channel') == channel] for rows in (left, right)]
        assert len(per_channel[0]) == len(per_channel[1]), 'Channel sound event count changed'
        for a, b in zip(*per_channel):
            aa, bb = dict(a), dict(b)
            t48, t44 = aa.pop('sample_frame'), bb.pop('sample_frame')
            assert aa == bb, 'Ordered voice request changed with output rate'
            # Events identify the enclosing output frame. Their physical
            # times can differ by at most one sample at the lower rate.
            assert abs(t48 * 44100 - t44 * 48000) <= 48000, 'Sound handoff moved beyond its output interval'
            events += 1
    return events


def trace_rows(path):
    rows = [json.loads(line) for line in path.read_text().splitlines()]
    return rows[1:-1]


def controls(left, right):
    rejected = []
    boundary = next(i for i, row in enumerate(right) if row['kind'] == 'boundary')
    request = next(i for i, row in enumerate(right) if row['kind'] == 'request' and row['active'])
    for change in ('phase', 'event_time', 'payload', 'channel', 'lost_event'):
        damaged = list(right)
        index = boundary if change == 'phase' else request
        damaged[index] = copy.deepcopy(damaged[index])
        if change == 'phase': damaged[index]['channels'][0]['phase'] += 1
        elif change == 'event_time': damaged[index]['sample_frame'] += 100
        elif change == 'payload': damaged[index]['sha256'] = '0' * 64
        elif change == 'channel': damaged[index]['channel'] ^= 1
        else: del damaged[index]
        try:
            same_physical_sound(left, damaged)
        except AssertionError:
            rejected.append(change)
        else:
            raise AssertionError('Accepted changed sound observation: ' + change)
    return rejected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, help='Optional preceding native executable, including lossless .gz')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--startup-only', action='store_true', help='only qualify SDL device/poll startup and invalid rates')
    args = parser.parse_args()
    assert args.out.resolve().is_relative_to((ROOT / 'build').resolve()), 'Diagnostic output must stay inside build'
    args.out.mkdir(parents=True, exist_ok=True)
    report = dict(runner_sha256=sha(args.runner), cases={},
                  scope='Connected output rate qualification; complete game state and physical byte phases/ordered requests. No original/native full waveform acceptance.')
    work = Path(tempfile.mkdtemp(prefix='ram-output-rate-', dir=args.out)).resolve()
    assert work.is_relative_to((ROOT / 'build').resolve()), 'Diagnostic output must stay inside build'
    complete = False
    try:
        baseline = args.baseline
        if baseline and baseline.suffix == '.gz':
            baseline = work / 'baseline.exe'
            baseline.write_bytes(gzip.decompress(args.baseline.read_bytes()))
        if baseline:
            report['baseline_runner_sha256'] = sha(baseline)
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\nF 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        flight = work / 'flight.e9k'
        flight.write_text('E9K_INPUT_V1\n' + ''.join(
            f'F {frame} K {key} 0 0 1\nF {frame+2} K {key} 0 0 0\n'
            for frame, key in ((1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49))))
        final_input = ROOT / load_tour_result()[1]['input']

        peak_capture_bytes = 0

        def run(runner, name, frames, replay, pilot, rate=None, observe=True, window=False):
            nonlocal peak_capture_bytes
            folder = work / name; folder.mkdir()
            if pilot:
                (folder / 'config').write_bytes(pilot)
            command = [str(runner.resolve()), '--frames', str(frames),
                       '--save-dir', str(folder), '--replay', str(replay), '--wav', str(folder / 'audio.wav'),
                       '--data-out', str(folder / 'data.dat'), '--ppm', str(folder / 'pixels.ppm'),
                       '--memory-report', str(folder / 'memory.json')]
            command += ['--hidden', '--recorded-input-only', '--clock', 'pal'] if window else ['--headless']
            if rate:
                command += ['--audio-rate', str(rate)]
            if observe:
                command += ['--audio-trace', str(folder / 'audio.jsonl')]
            environment = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy') if window else None
            result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=120, env=environment)
            (args.out / (name + '.log')).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, result.stderr
            captured = sum(p.stat().st_size for p in work.rglob('*') if p.is_file())
            peak_capture_bytes = max(peak_capture_bytes, captured)
            assert peak_capture_bytes <= 512 * 1024 * 1024, 'Combined diagnostic capture exceeded 512 MiB'
            stats = json.loads(result.stdout)
            memory = json.loads((folder / 'memory.json').read_text())
            assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == memory['sdl_gameplay_pool_requests'] == 0
            with wave.open(str(folder / 'audio.wav')) as pcm:
                assert (pcm.getnchannels(), pcm.getsampwidth(), pcm.getframerate(), pcm.getnframes()) == (2, 2, rate or 48000, frames * (rate or 48000) // 50)
            trace = trace_rows(folder / 'audio.jsonl') if observe else None
            if observe:
                read_audio_trace(folder / 'audio.jsonl', stats, (folder / 'data.dat').read_bytes(), frames_per_tick=(rate or 48000) // 50)
            value = dict(stats=stats, memory=memory, **{key: sha(folder / file) for key, file in
                         [('wav_sha256', 'audio.wav'), ('data_sha256', 'data.dat'), ('ppm_sha256', 'pixels.ppm')]},
                         save_sha256=sha(folder / 'config') if (folder / 'config').exists() else None)
            if observe:
                value['audio_trace_sha256'] = sha(folder / 'audio.jsonl')
            return value, trace, folder

        cases = [] if args.startup_only else [('intro', 3100, intro, None), ('free-flight', 6500, flight, None),
                                              ('final-combat', 24200, final_input, load_tour_pilot(8))]
        for name, frames, replay, pilot in cases:
            a, ta, fa = run(args.runner, name + '-48000', frames, replay, pilot)
            b, tb, fb = run(args.runner, name + '-44100', frames, replay, pilot, 44100)
            for key in ('data_sha256', 'ppm_sha256', 'save_sha256'):
                assert a[key] == b[key], (name, key)
            assert {k: v for k, v in a['stats'].items() if k not in ('sample_frames', 'nonzero_sample_frames')} == {
                k: v for k, v in b['stats'].items() if k not in ('sample_frames', 'nonzero_sample_frames')}, name
            events = same_physical_sound(ta, tb)
            value = dict(rate_48000=a, rate_44100=b, same_complete_game_state=True,
                         same_physical_byte_phases=True, matched_ordered_sound_events_per_channel=events,
                         actual_observation_mutations_rejected=controls(ta, tb))
            captured_bytes = sum(p.stat().st_size for p in work.rglob('*') if p.is_file())
            assert captured_bytes <= 512 * 1024 * 1024, 'Combined diagnostic capture exceeded 512 MiB'
            value['combined_capture_bytes'] = captured_bytes
            if baseline:
                old, _, fo = run(baseline, name + '-baseline', frames, replay, pilot, observe=False)
                assert {k: v for k, v in old.items() if k != 'memory'} == {
                    k: v for k, v in a.items() if k not in ('memory', 'audio_trace_sha256')}, name
                value['default_preserves_preceding_native'] = True
                shutil.rmtree(fo)
            if name == 'intro':
                explicit, _, fe = run(args.runner, name + '-explicit-48000', frames, replay, pilot, 48000)
                assert explicit == a, 'Explicit default rate changed output'
                value['explicit_default_output_identical'] = True
                shutil.rmtree(fe)
            report['cases'][name] = value
            shutil.rmtree(fa); shutil.rmtree(fb)
            print(f'{name}: complete game state, physical phases and {events} per-channel ordered sound events match; zero gameplay heap/pool requests', flush=True)
        report['device_startup'] = {}
        for rate in (44100, 48000):
            headless, _, fh = run(args.runner, f'headless-startup-{rate}', 120, intro, None, rate, observe=False)
            device, _, fd = run(args.runner, f'device-startup-{rate}', 120, intro, None, rate, observe=False, window=True)
            assert device['stats'] == dict(headless['stats'], audio_device=True)
            for key in ('wav_sha256', 'data_sha256', 'ppm_sha256', 'save_sha256'):
                assert device[key] == headless[key], ('Device changed complete startup output', rate, key)
            report['device_startup'][rate] = dict(device=device, complete_output_matches_headless=True,
                                                 drivers='SDL dummy audio/video; actual WASAPI qualification is separate')
            shutil.rmtree(fh); shutil.rmtree(fd)
            print(f'{rate} Hz: complete device/headless startup output matches; zero gameplay heap/pool requests', flush=True)
        rejected = []
        for text in ('0', '22050', '48000x', '-44100', '44100.0'):
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '1', '--audio-rate', text],
                                    cwd=ROOT, capture_output=True, text=True, timeout=10)
            assert result.returncode and 'Audio rate must be' in result.stderr
            rejected.append(text)
        report['invalid_startup_rates_rejected'] = rejected
        report['peak_combined_capture_bytes'] = peak_capture_bytes
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        complete = True
    finally:
        if complete:
            shutil.rmtree(work)
        else:
            (args.out / 'failure.json').write_text(json.dumps(dict(retained_case=str(work), partial_report=report), indent=2) + '\n')


if __name__ == '__main__':
    main()
