"""Measure visible mission gameplay with sound and ordinary camera keys.

An earned pilot and sealed flight route admit the mission. Added G and keypad
events exercise gear and every source camera mode without changing game state
directly. A fresh headless run supplies exact RAM/PCM/counter expectations for
the visible replay. This is a performance check, not original whole-flight
parity or mission-completion acceptance.
"""
import argparse
import csv
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess

from report_frame_times import report, summarize
from tour_pilot_fixture import load_tour_pilot, load_tour_result, tour_input

ROOT = Path(__file__).resolve().parents[2]


def sha(path):
    with path.open('rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def flight_route(mode):
    if mode == 8:
        source = ROOT / load_tour_result()[1]['input']
    else:
        source = tour_input(mode)
    return load_tour_pilot(mode), source


def camera_input(source, frames):
    lines = source.read_text().splitlines()
    assert lines[0] == 'E9K_INPUT_V1'
    events = []
    for line in lines[1:]:
        fields = line.split()
        assert len(fields) == 7 and fields[0] == 'F' and fields[2] == 'K'
        if int(fields[1]) < frames:
            events.append((int(fields[1]), line))
    # Key identities come from host_keys.c and command_selection.c. KP6
    # increments 0..11, KP7 selects 12, KP1 selects 13, KP8 returns to 0.
    extra = [(10700, 103)]
    target_presses = [tick for tick, line in events if line.split()[3:] == ['116', '0', '0', '1']]
    for view in range(1, 12):
        start = 11000 + 800*(view-1)
        # Target selection can reset the source camera. Begin each explicit
        # keypad sequence after nearby target presses; do not alter the route.
        nearby = [tick for tick in target_presses if start <= tick <= start+112]
        if nearby:
            start = nearby[-1]+12
        extra.append((start, 264))
        extra += [(start+8*(i+1), 262) for i in range(view)]
    extra += [(19800, 263), (20600, 257), (21400, 264)]
    assert frames > extra[-1][0] + 100
    for tick, key in extra:
        events.extend([(tick, f'F {tick} K {key} 0 0 1'),
                       (tick+2, f'F {tick+2} K {key} 0 0 0')])
    events.sort(key=lambda item: item[0])  # Preserve sealed ordering at equal ticks.
    return 'E9K_INPUT_V1\n' + '\n'.join(line for _, line in events) + '\n'


def trace_scope(path, mode):
    with path.open() as file:
        header = json.loads(next(file))
        assert header['format'] == 'FA18_FLIGHT_TRACE_V2'
        names = {field['name']: i for i, field in enumerate(header['fields'])}
        views, first_raised, last_lowered, airborne, missiles, aircraft = {}, None, None, 0, 0, 0
        rows, last_frame = 0, 0
        for line in file:
            row = json.loads(line)
            if row.get('end'):
                assert row['rows'] == rows and not file.read(1)
                break
            rows += 1
            assert row['frame'] >= last_frame
            last_frame = row['frame']
            assert len(row['records']) == 16
            records = [bytes.fromhex(value) for value in row['records']]
            assert all(len(record) == 164 for record in records)
            if int(row['fields'][names['mode']], 16) != mode:
                continue
            view = int(row['fields'][names['view_mode']], 16)
            views[str(view)] = views.get(str(view), 0) + 1
            contact = int.from_bytes(records[0][2:4], 'big')
            stage = int(row['fields'][names['stage']], 16)
            if stage == 0xc10dae and not contact & 0x80:
                airborne += 1
                if records[0][124] & 0x80:
                    first_raised = first_raised or row['frame']
                elif first_raised is not None:
                    last_lowered = row['frame']
            active = [record for record in records[1:] if int.from_bytes(record[:2], 'big') & 0x40]
            missiles += any(record[98] in (0, 1) for record in active)
            aircraft += any(record[98] in (0x11, 0x12) for record in active)
        else:
            raise AssertionError('Missing flight trace footer')
    assert airborne and first_raised and last_lowered is None, 'The combat clip must fly with gear raised'
    assert set(views) == {str(view) for view in range(14)}, views
    return dict(rows=rows, views=views, airborne_observations=airborne,
                first_raised_gear_frame=first_raised, airborne_gear_lowered_after_raise=last_lowered,
                active_missile_observations=missiles, active_aircraft_observations=aircraft)


def retain(work, evidence):
    """Keep reports/compact reusable evidence after complete file equality."""
    assert evidence['accepted_visible_combat_views']
    duplicate = []
    for name in ('final.dat', 'audio.wav', 'pixels.ppm', 'config'):
        first, second = work / 'headless' / name, work / 'visible' / name
        assert first.read_bytes() == second.read_bytes(), name
        assert sha(first) == evidence['headless']['hashes'][name]
        duplicate.append('visible/'+name)
    files = []
    for name in ('headless/final.dat', 'headless/flight.jsonl'):
        path = work / name
        raw = path.read_bytes()
        encoded = gzip.compress(raw, mtime=0)
        target = path.with_name(path.name+'.gz')
        target.write_bytes(encoded)
        assert gzip.decompress(target.read_bytes()) == raw
        files.append(dict(path=str(target), decoded_bytes=len(raw),
            decoded_sha256=hashlib.sha256(raw).hexdigest(), retained_bytes=len(encoded),
            retained_sha256=sha(target)))
        path.unlink()
    # Complete passing WAVs are duplicate native output checks, rather than
    # new original audio authorities. Retain their hashes, not another pair.
    removed = duplicate + ['headless/audio.wav']
    for name in removed:
        (work / name).unlink()
    result = dict(scope='Whole PCM/RAM/pixels/save byte equality verified before removing passing duplicates. '
        'Retain every timing row, reports and compressed native diagnostic trace/RAM. Failing runs stay intact.',
        files=files, passing_duplicates_removed=removed, capture_budget_mib=512)
    (work / 'retention.json').write_text(json.dumps(result, indent=2)+'\n')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--mode', type=int, choices=(5, 6, 7, 8), default=8)
    parser.add_argument('--frames', type=int, default=21550)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native.exe')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--prepare-only', action='store_true', help='Validate the route headlessly and retain its expectation before opening a window')
    args = parser.parse_args()
    assert 21550 <= args.frames <= 24200, 'Bounded airborne combat/camera clip, before the landing approach'
    pilot_bytes, source = flight_route(args.mode)
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    input_path = work / 'cameras.e9k'
    replay = camera_input(source, args.frames)
    if input_path.exists():
        assert input_path.read_text() == replay
    else:
        input_path.write_text(replay)
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_',
        'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_TRACE_', 'FA18_MISSION_', 'FA18_CAMPAIGN_'))}
    assert env.get('SDL_VIDEODRIVER', '') not in ('dummy', 'offscreen')
    assert env.get('SDL_AUDIODRIVER', '') != 'dummy'
    commands = []

    def run(name, visible=False):
        folder = work / name
        folder.mkdir(exist_ok=False)
        (folder / 'config').write_bytes(pilot_bytes)
        command = [str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
            '--save-dir', str(folder), '--frames', str(args.frames), '--replay', str(input_path),
            '--recorded-input-only', '--data-out', str(folder / 'final.dat'),
            '--wav', str(folder / 'audio.wav'), '--ppm', str(folder / 'pixels.ppm'),
            '--memory-report', str(folder / 'memory.json'), '--frame-times', str(folder / 'times.csv')]
        if not visible:
            command += ['--headless', '--flight-trace', str(folder / 'flight.jsonl')]
        commands.append(command)
        with (folder / 'run.log').open('w') as log:
            result = subprocess.run(command, cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT,
                timeout=args.frames*.02+180 if visible else 90)
        stats, = [json.loads(line) for line in (folder / 'run.log').read_text().splitlines() if line.startswith('{')]
        memory = json.loads((folder / 'memory.json').read_text())
        record = dict(returncode=result.returncode, stats=stats, memory=memory,
            hashes={filename: sha(folder / filename) for filename in ('final.dat', 'audio.wav', 'pixels.ppm', 'config')})
        (folder / 'output.json').write_text(json.dumps(record, indent=2)+'\n')
        assert result.returncode == 0 and stats['frames'] == args.frames, record
        assert not any(stats[key] for key in ('cpu_emulation', 'chipset_emulation', 'postflight_resets', 'host_replay_pending', 'input_queued'))
        assert not memory['project_gameplay_heap_violations'] and not memory['sdl_failures']
        return record

    headless = work / 'headless/output.json'
    if headless.exists():
        before = json.loads(headless.read_text())
        prepared = json.loads((work / 'prepared.json').read_text())
        assert prepared['runner_sha256'] == sha(args.runner) and prepared['input_sha256'] == sha(input_path)
        assert prepared['mode'] == args.mode and prepared['frames'] == args.frames
        assert prepared['initial_pilot_sha256'] == hashlib.sha256(pilot_bytes).hexdigest()
    else:
        before = run('headless')
        scope = trace_scope(work / 'headless/flight.jsonl', args.mode)
        prepared = dict(mode=args.mode, frames=args.frames, runner_sha256=sha(args.runner),
            source_input=str(source.relative_to(ROOT)), source_input_sha256=sha(source),
            input_sha256=sha(input_path), initial_pilot_sha256=hashlib.sha256(pilot_bytes).hexdigest(),
            adf_sha256=sha(ROOT / 'local/media/fa18.adf'), commands=commands, flight_scope=scope)
        (work / 'prepared.json').write_text(json.dumps(prepared, indent=2)+'\n')
    if args.prepare_only:
        print(json.dumps(prepared['flight_scope']), flush=True)
        return
    print(f'Opening visible mission mode {args.mode}, all camera views, with sound: {args.frames*.02/60:.1f} minutes. '
          'Leave the window open; it closes automatically after the measured combat clip.', flush=True)
    after = run('visible', True)
    timing = report(work / 'visible/times.csv', 20000)
    with (work / 'visible/times.csv').open(newline='') as file:
        rows = list(csv.DictReader(file))
    scenes = [row for row in rows if row['scene_updated'] == '1' and int(row['mode']) == args.mode]
    by_view = {str(view): summarize([row for row in scenes if int(row['view']) == view], 20000) for view in range(14)}
    differences = {key: [value, after['stats'].get(key)] for key, value in before['stats'].items()
                   if key != 'audio_device' and value != after['stats'].get(key)}
    evidence = dict(scope='Paced visible partial mission combat/camera clip from an earned saved pilot; every SDL presentation counted. '
        'Compositor scanout, full mission outcome and original whole-flight parity are separate.',
        prepared=prepared, visible_commands=commands, headless=before, visible=after,
        timing=timing, scene_work_by_view=by_view, runtime_counter_differences=differences,
        complete_pcm_ram_pixels_save_exact=before['hashes'] == after['hashes'],
        all_frames_presented=timing['all']['frames'] == timing['all']['presented'] == args.frames,
        all_camera_views_rendered=all(item['frames'] for item in by_view.values()),
        within_work_budget=not timing['all']['over_work_budget'], native_runtime_changed=False)
    evidence['accepted_visible_combat_views'] = bool(not differences and evidence['complete_pcm_ram_pixels_save_exact'] and
        evidence['all_frames_presented'] and evidence['all_camera_views_rendered'] and evidence['within_work_budget'] and
        after['stats']['audio_device'] and after['stats']['nonzero_sample_frames'])
    (work / 'comparison.json').write_text(json.dumps(evidence, indent=2)+'\n')
    assert evidence['accepted_visible_combat_views'], 'Inspect retained comparison and every failing frame; no exclusions applied'
    retain(work, evidence)
    print(f"{args.frames} visible presentations; all 14 views; exact PCM/RAM/pixels/save/counters; "
          f"maximum work {timing['all']['phases']['work_us']['max_ms']:.4f} ms", flush=True)


if __name__ == '__main__':
    main()
