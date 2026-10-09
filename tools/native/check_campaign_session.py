"""Earn qualification and six missions with one frontend and one playable process.

The keyboard driver observes live state and emits ordinary host events. Each
campaign starts with the actual normally enlisted save; flight state is never
seeded. Original whole-flight parity is a separate acceptance item.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import shutil
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_campaign_session_test.exe')
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/campaign-session')
    parser.add_argument('--timeout', type=float, default=600, help='Deadline for each complete continuous run, seconds')
    parser.add_argument('--live-enlistment', action='store_true', help='Enlist inside the campaign process, retaining the original ADF startup difficulty')
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    fixtures = ROOT / 'tools/native/fixtures'
    region = json.loads((fixtures / 'region-flight-pilot.json').read_text())
    adf = ROOT / 'local/media/fa18.adf'
    assert sha(adf.read_bytes()) == region['adf_sha256']
    qualification = fixtures / 'carrier_qualification_game_input.fa18in'
    enlist = fixtures / 'region-pilot-enlist.e9k'
    for path in (qualification, enlist):
        assert sha(path.read_bytes().replace(b'\r\n', b'\n')) == region['input_sha256_lf'][path.relative_to(ROOT).as_posix()]

    def run(name, command):
        log = work / f'{name}.log'
        with log.open('w') as output:
            subprocess.run(list(map(str, command)), cwd=ROOT,
                stdout=output, stderr=subprocess.STDOUT, text=True,
                timeout=args.timeout, check=True)
        return [json.loads(line) for line in log.read_text().splitlines() if line.startswith('{')]

    with tempfile.TemporaryDirectory(prefix='continuous-', dir=work) as temporary:
        pilot = Path(temporary)
        replay = work / 'session.e9k'
        extra = []
        if not args.live_enlistment:
            # Start qualification with the actual normally enlisted save. The
            # enrollment process is outside the uninterrupted flight campaign.
            enlisted, = run('enlist', [args.runner.resolve(), '--adf', adf, '--save-dir', pilot / 'enlist',
                '--headless', '--frames', '9000', '--replay', enlist])
            assert enlisted['screen'] == 'menu' and enlisted['mode'] == 0, enlisted
            earned = (pilot / 'enlist/config').read_bytes()
            assert sha(earned) == region['initial_config_sha256']
            for destination in ('driver', 'playable'):
                (pilot / destination).mkdir()
                shutil.copyfile(pilot / 'enlist/config', pilot / destination / 'config')
            extra = ['saved-pilot']
        evidence = run('driver', [args.test.resolve(), adf, pilot / 'driver', qualification, enlist, replay, *extra])
        checkpoints = [item for item in evidence if item.get('checkpoint')]
        missions = [item for item in evidence if 'mission' in item]
        summary, = [item for item in evidence if item.get('continuous_campaign')]
        assert summary['frontend_opens'] == 1 and summary['completions'] == 6 and summary['menu'] and not summary['crash_resets'], summary
        assert [item['mode'] for item in checkpoints] == [0, 9, 3, 4, 5, 6, 7, 8], checkpoints
        assert [item['mission'] for item in missions] == list(range(3, 9)), missions
        initial = bytes.fromhex(checkpoints[0]['saved_config_hex'])
        assert sha(initial) == region['initial_config_sha256']
        previous_tick = 0
        for checkpoint in checkpoints:
            saved = bytes.fromhex(checkpoint['saved_config_hex'])
            assert len(saved) == 78 and checkpoint['tick'] > previous_tick
            previous_tick = checkpoint['tick']
            mode = checkpoint['mode']
            assert saved[:2] == (b'\0\0' if mode == 0 else b'\0\1')
            earned = max(0, mode - 2) if mode != 9 else 0
            assert int.from_bytes(saved[56:58], 'big') == earned
            assert saved[21:27] == b'\1' * earned + b'\0' * (6 - earned)
            if mode in range(3, 9):
                assert checkpoint['stage'] == 'C0FCB4'
                assert saved[6] == mode
        for mission in missions:
            assert all(mission[field] for field in ('airborne', 'objective', 'landed', 'menu',
                                                   'gear_raised_in_flight', 'gear_lowered_for_landing')), mission
            if mission['mission'] != 3:
                assert mission['wire'], mission
            assert mission['completions'] == mission['mission'] - 2
        final = bytes.fromhex(checkpoints[-1]['saved_config_hex'])
        canonical, = run('playable', [args.runner.resolve(), '--adf', adf, '--save-dir', pilot / 'playable',
            '--headless', '--frames', summary['ticks'], '--replay', replay])
        assert (pilot / 'playable/config').read_bytes() == final, 'Playable continuous save differs from driver'
        assert canonical['screen'] == 'menu' and canonical['stage'] == 'C0FCB4' and canonical['mode'] == 0, canonical
        assert canonical['frames'] == summary['ticks'] and canonical['scene_frames'] == summary['scene_frames'], canonical
        assert not any(canonical[key] for key in ('cpu_emulation', 'chipset_emulation', 'postflight_resets', 'host_replay_pending', 'input_queued')), canonical
        assert canonical['host_replay_events'] == len(replay.read_text().splitlines()) - 1, canonical
    report = {'single_process_campaign': True, 'pilot_newly_enlisted': True,
        'native_flight_state_seeded': False, 'pilot_config_synthesized_by_checker': False,
        'initial_config_copied_from_actual_enlistment_save': not args.live_enlistment,
        'enlistment_inside_campaign_process': args.live_enlistment,
        'reference_scope': 'Continuous native campaign and playable replay; independent original whole-flight comparison remains open',
        'driver_sha256': sha(args.test.read_bytes()), 'runner_sha256': sha(args.runner.read_bytes()),
        'adf_sha256': sha(adf.read_bytes()), 'input_sha256_lf': sha(replay.read_bytes().replace(b'\r\n', b'\n')),
        'checkpoints': checkpoints, 'missions': missions, 'summary': summary, 'canonical': canonical,
        'saved_config_sha256': sha(final)}
    (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'One running session earns qualification and all six missions; {summary["ticks"]} ticks, all saved bytes and scene frames agree with fa18_native')


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
