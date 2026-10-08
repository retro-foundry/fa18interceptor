"""Compare normal-key mission objectives, landing, results and optional restart.

Original instructions receive native before-states in a separate process.
The playable runtime receives only ADF assets and keyboard input.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

from capture_workspace import CaptureWorkspace
from check_mission_five_objective import objective_evidence
from mission_source_comparison import compare_mission_boundaries
from region_pilot_fixture import load_region_pilot
from final_sequence_capture import collect_final_sequence

ROOT = Path(__file__).resolve().parents[2]


def rescue_evidence(exports, bodies, entries, objective):
    events = [item for item in exports if item.get('rescue_event')]
    launch, = [item for item in events if item['launch']]
    contact, = [item for item in events if item['contact']]
    success, = [item for item in events if item['phase_before'] == 0 and item['phase_after'] == 255]
    assert launch['pod_offset'] in (512, 1024, 1536) and launch['kind'] == 0x31, launch
    assert launch['timer_after'] == -19 and not launch['flags_after'] & 0x8000, launch
    assert launch['body'] < contact['body'] < success['body'], events
    assert contact['flags_after'] & 0x8000 and contact['pod_fixed'][1] == 0, contact
    assert success['pod_offset'] == launch['pod_offset'] == contact['pod_offset'], events
    assert success['flags_after'] & 0x8000 and success['timer_after'] == 0 and success['sequence_phase'] == 3, success
    assert success['body'] == objective['body'] and not any(item['phase_after'] == 254 for item in events), events
    distances = [abs(success['site_fixed'][axis] - success['pod_fixed'][axis]) for axis in (0, 2)]
    assert all(distance <= 0x10000 for distance in distances), distances
    compared = {item['body'] for item in bodies if item['rescue_window']}
    for event in (launch, contact):
        assert set(range(event['body'], event['body'] + 20)) <= compared, event
    deploy, = [item for item in entries if 35 in item['keys'] and item['modifier_before'] == 1 and item['control_after'] & 15 == 15]
    assert deploy['tick'] < launch['tick'], deploy
    return {'launch': launch, 'contact': contact, 'success': success, 'deploy_input': deploy,
            'distance_fixed_xz': distances, 'compared_rescue_bodies': len(compared)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mission_success_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mission-success-check')
    parser.add_argument('--mode', type=int, choices=(3, 4, 5, 6, 8), default=3)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe',
                        help='Playable runner for escort, rescue and final-mission replays')
    parser.add_argument('--sequence', action='store_true', help='Finish result messages and press Escape to restart into the menu')
    parser.add_argument('--timeout', type=float, default=60,
                        help='Native fixture deadline in seconds (Debug capture runs can require longer)')
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    if args.mode in (6, 8) and not args.sequence:
        parser.error('Rescue and final missions use --sequence for complete-flight comparison')
    work = args.out.resolve()
    env = os.environ.copy()
    env['FA18_MISSION_END_TICK'] = '32000' if args.mode == 3 else '30000'
    if args.mode == 6:
        env['FA18_MISSION_END_TICK'] = '40000'
    with CaptureWorkspace(work) as ram:
        prefix, pilot = ram / 'frame', ram / 'pilot'
        keys = work / 'pilot.e9k'
        scenario = f'{args.mode}-sequence' if args.sequence else '3' if args.mode == 3 else f'{args.mode}-success'
        initial = None
        if args.mode == 4:
            initial = load_region_pilot()
            pilot.mkdir()
            (pilot / 'config').write_bytes(initial)
        writes = partitions = None
        if args.mode == 8:
            exports, keys, initial, saved, writes, partitions = collect_final_sequence(args, work, ram, env)
        else:
            replay = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                                     str(pilot), str(keys), str(prefix), scenario],
                                    cwd=ROOT, capture_output=True, text=True, env=env, timeout=args.timeout)
            (work / 'native.log').write_text(replay.stdout + replay.stderr)
            assert replay.returncode == 0, replay.stderr or replay.stdout
            exports = [json.loads(line) for line in replay.stdout.splitlines() if line.startswith('{')]
            (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
        entries = [item for item in exports if 'entry' in item and not item.get('wrap_probe')]
        bodies = [item for item in exports if 'capture' in item and not item.get('wrap_probe')]
        summaries = [item for item in exports if item.get('finished')]
        assert len(summaries) == 1, summaries
        summary = summaries[0]
        assert summary['airborne'] and summary['landed'] and summary['objective'] and summary['reloaded'], summary
        assert summary['mode'] == args.mode and summary['phase'] == 252 and not summary['crash_resets'], summary
        assert summary['completions_after'] == summary['completions_before'] + 1, summary
        assert summary['grade_after'] == min(summary['grade_before'] + 1, 3), summary
        region_mask = 4 if args.mode == 3 else 0xc0
        assert summary['region'] & region_mask and not summary['speed'], summary
        landings = [item for item in bodies if item['touchdown']]
        assert len(landings) == 1, landings
        window = [item for item in bodies if item['landing_window']]
        assert [item['body'] for item in window] == list(range(landings[0]['body'], landings[0]['body'] + len(window)))
        evidence = None
        if args.sequence:
            assert len(window) == (80 if args.mode == 6 else 64), 'Missing sequence landing window'
        elif args.mode == 3:
            assert len(window) == 96, 'Missing landing window'
        if args.mode in (4, 5):
            if args.mode == 5:
                evidence = objective_evidence(exports, 4)
            else:
                escorts = [item for item in exports if item.get('escort_objective')]
                assert len(escorts) == 1, escorts
                escort = escorts[0]
                assert escort['flags'] & 0x40 and escort['contact'] & 0x80, escort
                assert any(escort['cell']) and escort['speed'] <= 0x320, escort
                assert not any(flags & 0x40 for flags in escort['enemy_flags']), escort
                hits = [item for item in exports if item.get('weapon_hit')]
                assert len(hits) == 1 and hits[0]['radar_after'] == hits[0]['radar_before'] + 1, hits
                assert any(item['slot'] == 8 and item['flags_before'] & 0x2000 and
                           item['flags_after'] & 0x400 and item['lifetime'] == 15 for item in hits[0]['records']), hits
                combat = [item for item in bodies if item['combat_window']]
                assert len(combat) == 20 and [item['body'] for item in combat] == list(range(hits[0]['body'], hits[0]['body'] + 20)), combat
                assert any(item['enemy_expiries_after'] == item['enemy_expiries_before'] + 1 for item in combat), combat
                evidence = {'escort': escort, 'radar_hit': hits[0], 'combat_window_bodies': len(combat)}
        if args.mode in (4, 5, 6, 8):
            returns = [item for item in exports if item.get('return_start')]
            assert len(returns) == 1 and returns[0]['pose'] == 3, returns
            assert landings[0]['region_after'] & 0xc0, 'Touchdown must be on the carrier'
            assert landings[0]['contact_before'] & 0x8000, 'Arrestor not deployed'
            if args.mode == 6:
                wire, = [item for item in window if not item['contact_before'] & 0x4000 and item['contact_after'] & 0x4000]
                assert wire['contact_after'] & 0xc080 == 0xc080 and wire['region_after'] & 0xc0, wire
                assert landings[0]['body'] <= wire['body'] <= window[-1]['body'], wire
            else:
                assert landings[0]['contact_after'] & 0xc080 == 0xc080, 'Missing wire capture and grounded contact'
            if not args.sequence:
                assert window[-1]['body'] == bodies[-1]['body'], 'Missing consecutive touchdown-to-result bodies'
            assert window[-1]['completions_after'] == summary['completions_after'], 'Result not covered by landing window'
        confirmations = [item for item in bodies if item['confirmation_before'] != item['confirmation_after']]
        assert confirmations and any(item['confirmation_after'] == 255 and item['selected'] >= 0 and
                                     item['view_record'] == 0 for item in confirmations), confirmations
        objectives = [item for item in bodies if item['phase_before'] == 0 and item['phase_after'] == 255]
        assert len(objectives) == 1, objectives
        if args.mode == 4:
            assert evidence['escort']['body'] == objectives[0]['body'], evidence
        rescue = rescue_evidence(exports, bodies, entries, objectives[0]) if args.mode == 6 else None
        finishes = [item for item in bodies if item['phase_after'] == 252 and item['phase_before'] != 252]
        assert len(finishes) == 1 and finishes[0]['ready'], finishes
        results = [item for item in entries if item['stage'] == 'C110A4' and
                   item['phase_before'] == 252 and item['completions'] == summary['completions_after']]
        assert len(results) == 1, results
        sequence_evidence = None
        if args.sequence:
            sequences = [item for item in exports if item.get('sequence')]
            assert len(sequences) == 1, sequences
            sequence = sequences[0]
            assert sequence['restarted'] and sequence['menu'] and sequence['log_unchanged'], sequence
            assert sequence['stage'] == 'C0FCB4' and sequence['mode'] == args.mode and sequence['phase'] == 0, sequence
            assert not sequence['queued'] and not sequence['crash_resets'], sequence
            assert sequence['completions'] == summary['completions_after'] and sequence['grade'] == summary['grade_after'], sequence
            messages = [item for item in exports if item.get('result_messages_finished')]
            assert len(messages) == 1 and messages[0]['phase'] == 4 and messages[0]['message_state'] < 0, messages
            assert messages[0]['restart_key'] == 27 and summary['ticks'] < messages[0]['tick'] < sequence['ticks'], messages
            message_returns = [item for item in entries if item['phase_before'] == 252 and item['phase_after'] == 4]
            assert len(message_returns) == 1, message_returns
            restart_keys = [item for item in entries if 69 in item['keys'] and item['tick'] > summary['ticks']]
            restart_releases = [item for item in entries if 197 in item['keys'] and item['tick'] > summary['ticks']]
            assert len(restart_keys) == len(restart_releases) == 1, (restart_keys, restart_releases)
            bootstrap = [item for item in entries if item['stage'] == 'C0F992']
            assert len(bootstrap) == 1 and bootstrap[0]['phase_before'] == 4 and bootstrap[0]['phase_after'] == 0, bootstrap
            required = {'C0F946', 'C0F974', 'C0F992'}
            assert required <= {item['stage'] for item in entries if item['tick'] > summary['ticks']}, entries
            assert bodies[-1]['stage'] == 'C0FCB4' and bodies[-1]['completions_after'] == sequence['completions'], bodies[-1]
            message_bodies = [item for item in bodies if item['before_tick'] > summary['ticks'] and
                              (item['message_b_before'] != item['message_b_after'] or
                               item['message_c_before'] != item['message_c_after'])]
            assert any(item['message_c_after'] == 128 for item in message_bodies), message_bodies
            sequence_evidence = {'summary': sequence, 'messages_finished': messages[0],
                                 'message_return': message_returns[0], 'restart_key': restart_keys[0],
                                 'restart_release': restart_releases[0], 'bootstrap': bootstrap[0],
                                 'message_body_transitions': message_bodies}
        if args.mode != 8:
            saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78 and int.from_bytes(saved[56:58], 'big') == summary['completions_after']
        assert saved[6] == args.mode and saved[7] == summary['grade_before'] and saved[18 + args.mode] == summary['grade_after']
        if writes is None:
            writes = compare_mission_boundaries(prefix, entries, bodies, work)
        assert writes == 1, f'Expected the actual mission config write; observed {writes}'
        report = {'scenario': {3: 'normal-key-mode-three-objective-landing-taxi-result-reload',
                              4: 'normal-key-mode-four-escort-carrier-landing-result-reload',
                              5: 'normal-key-mode-five-formation-radar-kills-carrier-landing-result-reload',
                              6: 'normal-key-mode-six-rescue-pod-carrier-landing-result-reload',
                              8: 'normal-key-final-patrol-carrier-landing-result-reload'}[args.mode],
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
        if evidence is not None:
            report['combat_objective'] = evidence
            report['return'] = returns[0]
            report['mission_success_accepted'] = True
        if rescue is not None:
            report.update({'rescue_objective': rescue, 'return': returns[0],
                           'wire_capture': wire,
                           'mission_success_accepted': True,
                           'initial_pilot_source': 'Unmodified pilot log from original ADF; port-earned progression remains open'})
        if sequence_evidence is not None:
            report['scenario'] = report['scenario'].replace('result-reload', 'result-messages-escape-menu-reload')
            report['result_sequence'] = sequence_evidence
            report['mission_sequence_accepted'] = True
        if args.mode in (4, 6, 8):
            canonical_pilot = ram / 'canonical-pilot'
            canonical_pilot.mkdir()
            if initial is not None:
                (canonical_pilot / 'config').write_bytes(initial)
            frames = sequence_evidence['summary']['ticks'] if args.sequence else summary['ticks']
            canonical = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                '--save-dir', str(canonical_pilot), '--headless', '--frames', str(frames), '--replay', str(keys)],
                cwd=ROOT, capture_output=True, text=True, timeout=args.timeout)
            (work / 'canonical.log').write_text(canonical.stdout + canonical.stderr)
            assert canonical.returncode == 0, canonical.stderr or canonical.stdout
            rows = [json.loads(line) for line in canonical.stdout.splitlines() if line.startswith('{')]
            assert len(rows) == 1, rows
            state = rows[0]
            assert not state['postflight_resets'] and not state['cpu_emulation'] and not state['chipset_emulation'], state
            assert state['host_replay_events'] == len(keys.read_text().splitlines()) - 1, state
            assert not state['host_replay_pending'] and not state['input_queued'], state
            assert (canonical_pilot / 'config').read_bytes() == saved, 'Playable saved result differs'
            if args.sequence:
                assert state['screen'] == 'menu' and state['stage'] == 'C0FCB4', state
            report['canonical'] = state
            if initial is not None:
                report['initial_config_sha256'] = hashlib.sha256(initial).hexdigest()
            report['runner_sha256'] = hashlib.sha256(args.runner.read_bytes()).hexdigest()
        if args.mode == 8:
            counters = [item for item in exports if item.get('final_mission_counter') and not item.get('wrap_probe')]
            expiries = [item for item in counters if item['expiries_before'] != item['expiries_after']]
            assert len(expiries) == 4 and [item['expiries_after'] for item in expiries] == [1, 2, 3, 4], expiries
            assert all(item['admitted_before'] == item['admitted_after'] == 4 for item in expiries), expiries
            objective, = [item for item in counters if item['phase_before'] == 0 and item['phase_after'] == 255]
            assert objective['expiries_after'] == 4 and objective['sequence_phase'] == 3, objective
            assert objective['records'][-1]['slot'] == 14 and objective['records'][-1]['kind'] == 0x20 and objective['records'][-1]['flags_after'] & 0x40, objective
            hits = [item for item in exports if item.get('weapon_hit')]
            assert len(hits) == 6 and hits[-1]['infrared_after'] == hits[0]['infrared_before'] + 2, hits
            assert hits[-1]['radar_after'] == hits[0]['radar_before'] + 2 and hits[-1]['gun_after'] == 2, hits
            compared_combat = {item['body'] for item in bodies if item['combat_window']}
            for hit in hits:
                assert set(range(hit['body'], hit['body'] + 20)) <= compared_combat, hit
            for expiry in expiries:
                assert any(item['body'] == expiry['body'] for item in bodies), expiry
            assert any(item['body'] == objective['body'] for item in bodies), objective
            wrap, = [item for item in exports if item.get('next_mission_wrap')]
            assert wrap['mode'] == 3 and wrap['saved_mode'] == 8 and wrap['result_fields_unchanged'] and not wrap['queued'], wrap
            assert wrap['enlistment_after'] == wrap['enlistment_before'] + 1, wrap
            canonical_wrap = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                '--save-dir', str(canonical_pilot), '--headless', '--frames', str(wrap['ticks']),
                '--replay', str(keys) + '.wrap.e9k'], cwd=ROOT, capture_output=True, text=True, timeout=args.timeout)
            (work / 'canonical-wrap.log').write_text(canonical_wrap.stdout + canonical_wrap.stderr)
            assert canonical_wrap.returncode == 0, canonical_wrap.stderr
            wrap_state, = [json.loads(line) for line in canonical_wrap.stdout.splitlines() if line.startswith('{')]
            assert wrap_state['mode'] == 3 and wrap_state['stage'] == wrap['stage'] and wrap_state['screen'] == wrap['screen'], wrap_state
            assert wrap_state['host_replay_events'] == 4 and not wrap_state['host_replay_pending'] and not wrap_state['input_queued'], wrap_state
            assert not wrap_state['postflight_resets'] and (canonical_pilot / 'config').read_bytes() == saved, wrap_state
            report.update({'mission_success_accepted': True, 'mission_availability_earned': False,
                'eligibility_fixture': 'tools/native/fixtures/final-mission-eligible-pilot.json',
                'capture_partitions': partitions, 'patrol_expiries': expiries, 'counter_objective': objective,
                'weapon_hits': hits, 'compared_combat_bodies': len(compared_combat),
                'return': returns[0], 'next_mission_wrap': wrap, 'canonical_wrap': wrap_state,
                'wrap_input_sha256_lf': hashlib.sha256(Path(str(keys) + '.wrap.e9k').read_text().encode()).hexdigest()})
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    suffix = ', result messages, Escape/menu restart and cold reload' if args.sequence else ''
    print(f"Mode {args.mode} mission: {len(entries)} input/stage intervals and {len(bodies)} bodies match original RAM/drawing, including all {len(window)} landing bodies and the actual config write{suffix}")


if __name__ == '__main__':
    main()
