"""Measure paced visible gameplay with audio and every SDL presentation.

The normal paths consume either an accepted independent mission flight or a
gear-managed campaign report, enlist through ordinary keys, then measure the
accepted replay visibly. Historical gear-down cases require an explicit flag.
Keep the window unobscured and allow the ordinary-input replay to finish.
"""
import argparse
import csv
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import shutil
import sys
import tempfile

from region_pilot_fixture import load_region_pilot
from tour_pilot_fixture import load_tour_pilot, load_tour_result, tour_input
from report_frame_times import report, summarize

ROOT = Path(__file__).resolve().parents[2]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def measure_mission(args):
    """Use the accepted independent mission-three recording and earned pilot.

    No campaign-success prerequisite, artificial pilot or flight RAM seed.
    The visible run must preserve every final RAM byte and runtime counter.
    """
    reference = json.loads(args.mission_report.read_text())
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    assert sha(args.runner.read_bytes()) == reference['runner_sha256']
    assert args.mission_input and sha(args.mission_input.read_bytes()) == reference['source_update_evidence']['remapped_input_sha256']
    for relative, expected in reference['input_hashes'].items():
        assert sha((ROOT / relative).read_bytes()) == expected, relative
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    adf = ROOT / 'local/media/fa18.adf'
    frames = reference['native_run']['frames']
    env = {k: v for k, v in os.environ.items() if not k.startswith(
        ('FA18_LOOP_', 'FA18_ORIGINAL_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_',
         'FA18_TRACE_', 'FA18_MISSION_'))}
    assert env.get('SDL_VIDEODRIVER', '') not in ('dummy', 'offscreen')
    assert env.get('SDL_AUDIODRIVER', '') != 'dummy'
    timing_path = work / 'independent-mission.csv'
    with tempfile.TemporaryDirectory(prefix='visible-mission-', dir=work) as temporary:
        pilot = Path(temporary) / 'pilot'
        enlist = ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'
        # Exact comparison to this retained flight uses its PAL clock contract.
        base = [str(args.runner.resolve()), '--clock', 'pal', '--adf', str(adf), '--save-dir', str(pilot)]
        with (work / 'enlist.log').open('w') as log:
            subprocess.run([*base, '--headless', '--frames', '9000', '--replay', str(enlist)],
                cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT, check=True, timeout=60)
        initial = (pilot / 'config').read_bytes()
        assert len(initial) == 78 and initial[:4] == bytes(4)
        assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2)
        intro = Path(temporary) / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        ram = Path(temporary) / 'final.dat'
        print(f'Opening visible independent qualification/mission-three replay with audio: '
              f'approximately {frames * .02 / 60:.1f} minutes', flush=True)
        print('Leave the window open through qualification and the menus. '
              'It closes automatically after mission three finishes; the checker then reports the result.', flush=True)
        with (work / 'independent-mission.log').open('w') as log:
            result = subprocess.run([*base, '--frames', str(frames), '--replay', str(intro), '--recorded-input-only',
                '--input', str(args.mission_input.resolve()), '--iterations',
                str(reference['source_update_evidence']['real_update_calls']),
                '--data-out', str(ram), '--frame-times', str(timing_path)],
                cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT,
                timeout=frames * .02 + 180)
        if result.returncode and ram.exists():
            (work / 'failed-final.dat.gz').write_bytes(gzip.compress(ram.read_bytes(), mtime=0))
        state, = [json.loads(s) for s in (work / 'independent-mission.log').read_text().splitlines() if s.startswith('{')]
        timing = report(timing_path, 20000)
        with timing_path.open(newline='') as file:
            rows = list(csv.DictReader(file))
        scene_by_mode = {str(mode): summarize([r for r in rows if int(r['mode']) == mode and r['scene_updated'] == '1'], 20000)
                         for mode in (3, 9)}
        views_by_mode = {str(mode): sorted({int(r['view']) for r in rows if int(r['mode']) == mode and r['scene_updated'] == '1'})
                         for mode in (3, 9)}
        mismatches = {key: [expected, state.get(key)] for key, expected in reference['native_run'].items()
                      if key != 'audio_device' and state.get(key) != expected}
        evidence = dict(scope='Attempted paced visible independent qualification/mission-three replay with sound. '
                             'Every SDL presentation is counted; compositor scanout is not instrumented. '
                             'Other missions and combat/view coverage remain separate.',
            visible_window_requested=True, hidden_window=False, pacing_modified=False,
            recorded_input_only=True,
            campaign_required=False, native_flight_state_seeded=False,
            runner_sha256=reference['runner_sha256'], adf_sha256=sha(adf.read_bytes()),
            mission_report_sha256=sha(args.mission_report.read_bytes()),
            input_sha256=sha(args.mission_input.read_bytes()), initial_config_sha256=sha(initial),
            final_ram_sha256=sha(ram.read_bytes()), final_saved_pilot=(pilot / 'config').read_bytes().hex(),
            canonical=state, timing=timing, mission_scene_work=scene_by_mode, mission_scene_views=views_by_mode,
            returncode=result.returncode,
            runtime_counter_differences=mismatches,
            expected_frames=frames, observed_frames_presented=timing['all']['presented'] == timing['all']['frames'] == state['frames'],
            saved_config_matches=(pilot / 'config').read_bytes().hex() == reference['final_saved_pilot'],
            final_ram_matches=sha(ram.read_bytes()) == reference['native_final_ram_sha256'],
            all_frames_presented=timing['all']['presented'] == timing['all']['frames'] == frames,
            within_work_budget=not timing['all']['over_work_budget'])
        evidence['accepted_complete_visible_flight'] = bool(result.returncode == 0 and not mismatches and evidence['saved_config_matches'] and
            evidence['final_ram_matches'] and evidence['all_frames_presented'] and evidence['within_work_budget'] and
            state['audio_device'] and state['nonzero_sample_frames'] and all(item['frames'] for item in scene_by_mode.values()))
        if not evidence['accepted_complete_visible_flight']:
            (work / 'failed-final.dat.gz').write_bytes(gzip.compress(ram.read_bytes(), mtime=0))
        (work / 'comparison.json').write_text(json.dumps(evidence, indent=2) + '\n')
        if result.returncode or mismatches or not evidence['saved_config_matches'] or not evidence['final_ram_matches']:
            raise RuntimeError(f"Visible replay does not reproduce the accepted complete flight "
                               f"({state['frames']}/{frames} frames); inspect retained comparison.json and failed-final.dat.gz")
        assert state['audio_device'] and state['nonzero_sample_frames'] and evidence['all_frames_presented']
        assert all(item['frames'] for item in scene_by_mode.values()), scene_by_mode
        if not evidence['within_work_budget']:
            raise RuntimeError('Visible independent flight exceeds 20 ms work; all failing rows are retained without exclusions')
        print(f'{frames} visible presentations; complete RAM, counters and earned save agree; all measured work within 20 ms')


def measure_campaign(args):
    reference = json.loads(args.campaign_report.read_text())
    assert reference['single_process_campaign'] and reference['summary']['completions'] == 6
    assert all(m['gear_raised_in_flight'] and m['gear_lowered_for_landing'] for m in reference['missions'])
    replay = args.campaign_report.parent / 'session.e9k'
    assert sha(replay.read_bytes().replace(b'\r\n', b'\n')) == reference['input_sha256_lf']
    assert sha(args.runner.read_bytes()) == reference['runner_sha256']
    adf = ROOT / 'local/media/fa18.adf'
    assert sha(adf.read_bytes()) == reference['adf_sha256']
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    frames = reference['summary']['ticks']
    timing_path = work / 'continuous-campaign.csv'
    with tempfile.TemporaryDirectory(prefix='geared-pilot-', dir=work) as temporary:
        pilot = Path(temporary)
        enlist = pilot / 'enlist'
        command = [args.runner.resolve(), '--clock', 'pal', '--adf', adf, '--save-dir', enlist,
            '--headless', '--frames', '9000', '--replay', ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k']
        with (work / 'enlist.log').open('w') as log:
            subprocess.run(list(map(str, command)), cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True, timeout=60)
        initial = (enlist / 'config').read_bytes()
        assert initial.hex() == reference['checkpoints'][0]['saved_config_hex']
        campaign = pilot / 'campaign'
        campaign.mkdir()
        if not reference['enlistment_inside_campaign_process']:
            shutil.copyfile(enlist / 'config', campaign / 'config')
        print(f'Opening visible gear-managed campaign with audio: approximately {frames * .02 / 60:.1f} minutes', flush=True)
        command = [args.runner.resolve(), '--clock', 'pal', '--adf', adf, '--save-dir', campaign,
            '--frames', frames, '--replay', replay, '--frame-times', timing_path]
        with (work / 'continuous-campaign.log').open('w') as log:
            subprocess.run(list(map(str, command)), cwd=ROOT, stdout=log, stderr=subprocess.STDOUT,
                check=True, timeout=frames * .02 + 180)
        states = [json.loads(s) for s in (work / 'continuous-campaign.log').read_text().splitlines() if s.startswith('{')]
        state, = states
        timing = report(timing_path, 20000)
        with timing_path.open(newline='') as file:
            rows = list(csv.DictReader(file))
        by_mode = {str(mode): summarize([r for r in rows if int(r['mode']) == mode and r['scene_updated'] == '1'], 20000)
            for mode in range(3, 10)}
        observed_views = {str(mode): sorted({int(r['view']) for r in rows if int(r['mode']) == mode and r['scene_updated'] == '1'})
            for mode in range(3, 10)}
        final = (campaign / 'config').read_bytes()
        evidence = {'scope': 'Paced visible qualification and six geared missions in one process, with audio; SDL returns do not instrument compositor scanout',
            'visible_window_requested': True, 'hidden_window': False, 'pacing_modified': False,
            'campaign_reference': str(args.campaign_report.resolve()), 'input_sha256_lf': reference['input_sha256_lf'],
            'runner_sha256': reference['runner_sha256'], 'canonical': state, 'timing': timing,
            'mission_scene_work': by_mode, 'mission_scene_views': observed_views,
            'saved_config_matches': final.hex() == reference['checkpoints'][-1]['saved_config_hex'],
            'all_frames_presented': timing['all']['presented'] == timing['all']['frames'] == frames,
            'within_work_budget': not timing['all']['over_work_budget']}
        (work / 'comparison.json').write_text(json.dumps(evidence, indent=2) + '\n')
        assert evidence['saved_config_matches'] and evidence['all_frames_presented'], evidence
        assert state['audio_device'] and state['nonzero_sample_frames']
        for key in ('screen', 'stage', 'mode', 'frames', 'scene_frames', 'host_replay_events', 'host_replay_pending', 'input_queued', 'postflight_resets'):
            assert state[key] == reference['canonical'][key], (key, state[key], reference['canonical'][key])
        assert all(item['frames'] for item in by_mode.values()), by_mode
        if not evidence['within_work_budget']:
            raise RuntimeError('Visible campaign exceeds 20 ms frame work; failing CSV/report retained without exclusions')
        print(f'{frames} visible presentations; qualification and six mission results agree; all measured frame work within 20 ms')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--mission-report', type=Path, help='Accepted check_mission_message_trace.py independent flight report.json')
    parser.add_argument('--mission-input', type=Path, help='Verified update-consumed.fa18in for the independent mission report')
    parser.add_argument('--campaign-report', type=Path, help='Accepted gear-managed check_campaign_session.py comparison.json')
    parser.add_argument('--allow-historical-gear-down-inputs', action='store_true',
        help='Reproduce the historical failed benchmark only; not current gear-managed acceptance')
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/visible-performance')
    args = parser.parse_args()
    if args.mission_report:
        if args.campaign_report or not args.mission_input:
            parser.error('--mission-report requires --mission-input and excludes --campaign-report')
        measure_mission(args)
        return
    if args.mission_input:
        parser.error('--mission-input requires --mission-report')
    if args.campaign_report:
        measure_campaign(args)
        return
    if not args.allow_historical_gear_down_inputs:
        parser.error('Historical mission inputs keep gear down. A validated gear-managed route is required before visible mission qualification; this historical reproduction requires --allow-historical-gear-down-inputs.')
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    fixtures = ROOT / 'tools/native/fixtures'
    region = json.loads((fixtures / 'region-flight-pilot.json').read_text())
    stolen = json.loads((fixtures / 'stolen-mission-pilot.json').read_text())
    later = json.loads((fixtures / 'new-pilot-tour-availability.json').read_text())
    final, result = load_tour_result()
    adf = ROOT / 'local/media/fa18.adf'
    assert sha(adf.read_bytes()) == region['adf_sha256'] == result['adf_sha256']
    evidence = {'visible_window_requested': True, 'hidden_window': False,
        'pacing_modified': False, 'native_flight_state_seeded': False,
        'scope': 'Visible demo and six cold earned mission routes with audio; SDL presentation returns are counted, compositor scanout is not instrumented',
        'runner_sha256': sha(args.runner.read_bytes()), 'adf_sha256': sha(adf.read_bytes()),
        'work_budget_ms': 20, 'cases': []}

    def run(name, pilot, frames, replay, extra=(), headless=False):
        # Keep these sealed routes on their explicit deterministic clock;
        # check_host_clock.py exercises default host-clock windowed gameplay.
        command = [str(args.runner.resolve()), '--clock', 'pal', '--adf', str(adf), '--save-dir', str(pilot),
                   '--frames', str(frames), '--replay', str(replay), *map(str, extra)]
        if headless:
            command += ['--headless']
        else:
            command += ['--frame-times', str(work / f'{name}.csv')]
        with (work / f'{name}.log').open('w') as log:
            subprocess.run(command, cwd=ROOT, stdout=log, stderr=subprocess.STDOUT,
                           check=True, timeout=frames * .02 + 120)
        state, = [json.loads(line) for line in (work / f'{name}.log').read_text().splitlines() if line.startswith('{')]
        assert not any(state[key] for key in ('cpu_emulation', 'chipset_emulation', 'postflight_resets', 'host_replay_pending', 'input_queued')), state
        return state

    with tempfile.TemporaryDirectory(prefix='pilots-', dir=work) as temporary:
        pilot = Path(temporary)
        intro = pilot / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 30 K 32 0 0 1\nF 32 K 32 0 0 0\n')
        demo = pilot / 'demo'
        demo.mkdir()
        cases = [('demo', demo, 30000, intro,
                  ('--input', ROOT / 'captures/native/demo01/input.fa18in'), None)]
        campaign = pilot / 'campaign'
        campaign.mkdir()
        run('enlist', campaign, 9000, fixtures / 'region-pilot-enlist.e9k', headless=True)
        assert sha((campaign / 'config').read_bytes()) == region['initial_config_sha256']
        acknowledge = pilot / 'acknowledge.e9k'
        acknowledge.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        run('qualification', campaign, 45000, acknowledge,
            ('--input', fixtures / 'carrier_qualification_game_input.fa18in', '--iterations', region['qualification_iterations']), headless=True)
        assert sha((campaign / 'config').read_bytes()) == region['qualification_config_sha256']
        cases.extend([
            ('mission-3', campaign, region['mission_three_menu_tick'], fixtures / 'region-pilot-mission-three.e9k', (), load_region_pilot()),
            ('mission-4', campaign, stolen['mission_four_menu_tick'], fixtures / 'mission-four-sequence.e9k', (), load_tour_pilot(5)),
            *[(f'mission-{mode}', campaign, later['missions'][str(mode + 1)]['previous_menu_tick'], tour_input(mode), (), load_tour_pilot(mode + 1)) for mode in (5, 6, 7)],
            ('mission-8', campaign, result['menu_tick'], ROOT / result['input'], (), final)])
        for name, savedir, frames, replay, extra, expected in cases:
            print(f'{name}: opening visible window with audio, up to {frames * .02 / 60:.1f} minutes', flush=True)
            initial = (savedir / 'config').read_bytes() if (savedir / 'config').exists() else None
            state = run(name, savedir, frames, replay, extra)
            assert state['audio_device'] and state['nonzero_sample_frames'], state
            if expected is not None:
                assert state['screen'] == 'menu' and state['stage'] == 'C0FCB4', state
                assert (savedir / 'config').read_bytes() == expected, name
            timing = report(work / f'{name}.csv', 20000)
            assert timing['all']['frames'] == state['frames']
            assert timing['all']['presented'] == state['frames'], name
            case = {'name': name, 'initial_config_sha256': sha(initial) if initial else None,
                    'input_sha256_lf': sha(replay.read_bytes().replace(b'\r\n', b'\n')),
                    'canonical': state, 'timing': timing}
            evidence['cases'].append(case)
            evidence['all_cases_finished'] = len(evidence['cases']) == len(cases)
            evidence['within_work_budget'] = all(not item['timing']['all']['over_work_budget'] for item in evidence['cases'])
            (work / 'comparison.json').write_text(json.dumps(evidence, indent=2) + '\n')
            totals = timing['all']
            print(f"{name}: {state['frames']} presentations, worst work {totals['phases']['work_us']['max_ms']} ms, {totals['over_work_budget']} frames over budget", flush=True)
    if not evidence['within_work_budget']:
        raise RuntimeError('Visible gameplay exceeds the 20 ms frame-work budget; inspect the retained CSV and worst-frame records')


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
