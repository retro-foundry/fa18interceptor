"""Check source modes 2, 3, 4, 5, 6, 7, 8 or 125 through native menu and flight.

Starts the shared playable runtime from disk/input and compares original
C0F3C4/C0F5F8 intervals plus C0EFEA/C0F3C0 bodies. No full Amiga replay.
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
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_mode_two_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/mode-check')
    parser.add_argument('--mode', type=int, choices=(2,3,4,5,6,7,8,125), default=2)
    parser.add_argument('--aircraft', type=int, choices=(1,2), default=1)
    parser.add_argument('--eject', action='store_true', help='Exercise Shift-E and mode-8 menu return')
    parser.add_argument('--weapon', type=int, choices=(1,2,3), help='Cycle Return 1/2/3 times and fire twice with Space')
    parser.add_argument('--flight', action='store_true', help='Mode-4 takeoff, weapon inputs and region/zone transitions')
    parser.add_argument('--callback', action='store_true', help='Free Flight Delete callback remove/reinstall')
    parser.add_argument('--smoothing', action='store_true', help='Select the source cancel marker at naturally reached mode-four smoothing')
    parser.add_argument('--combat', action='store_true', help='Mode-five through eight longer flight with manoeuvre-limit samples')
    parser.add_argument('--outcome', action='store_true', help='Mode-six normal-input failure, all three resets and menu return')
    parser.add_argument('--hit', action='store_true', help='Require a normal-input mode-eight weapon hit')
    parser.add_argument('--kill', action='store_true', help='Normal-input mode-eight missile destruction and expiry accounting')
    parser.add_argument('--missile', choices=('radar','infrared'), help='Missile for --hit/--kill (default: radar)')
    parser.add_argument('--gun', action='store_true', help='Gun for --hit')
    parser.add_argument('--gun-approach', action='store_true', help='Compare gun approach even when shots miss (diagnostic)')
    parser.add_argument('--keep-captures', action='store_true', help='Retain all raw RAM for deliberate debugging')
    args = parser.parse_args()
    if args.gun_approach:
        args.hit=True
        args.gun=True
    if args.kill:
        if args.hit:
            parser.error('--kill includes --hit; select one probe')
        args.hit=True
    if args.missile and not args.hit:
        parser.error('--missile requires --hit or --kill')
    if args.gun and (not args.hit or args.kill or args.missile):
        parser.error('--gun requires --hit without --kill/--missile')
    args.missile='gun' if args.gun else args.missile or 'radar'
    if args.eject and args.mode!=8:
        parser.error('--eject requires --mode 8')
    if args.weapon and (args.mode!=8 or args.eject):
        parser.error('--weapon requires --mode 8 without --eject')
    if args.flight and (args.mode!=4 or args.eject or args.weapon):
        parser.error('--flight requires --mode 4 without --eject/--weapon')
    if args.callback and (args.mode!=125 or args.flight or args.eject or args.weapon):
        parser.error('--callback requires --mode 125 without other probes')
    if args.smoothing and (args.mode!=4 or args.flight or args.eject or args.weapon or args.callback or args.combat):
        parser.error('--smoothing requires --mode 4 without other probes')
    if args.combat and (args.mode not in (5,6,7,8) or args.flight or args.eject or args.weapon or args.callback):
        parser.error('--combat requires --mode 5, 6, 7 or 8 without other probes')
    if args.outcome and (args.mode!=6 or args.combat or args.flight or args.eject or args.weapon or args.callback or args.smoothing):
        parser.error('--outcome requires --mode 6 without other probes')
    if args.hit and (args.mode!=8 or args.combat or args.flight or args.eject or args.weapon or args.callback or args.smoothing or args.outcome):
        parser.error('--hit requires --mode 8 without other probes')
    work = args.out.resolve()
    work.mkdir(parents=True, exist_ok=True)
    with CaptureWorkspace(work, args.keep_captures) as capture_dir:
        check(args, work, capture_dir)


def check(args, work, capture_dir):
    prefix = capture_dir / 'frame'
    hit_probe=(args.missile+'-' if args.missile!='radar' else '')+('kill' if args.kill else 'hit')
    if args.gun_approach:
        hit_probe='gun-approach'
    hit_weapon={'infrared':1,'radar':2,'gun':3}[args.missile]
    result = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                             str(work / 'pilot-test'), str(prefix), str(args.mode), str(args.aircraft),
                             *([hit_probe] if args.hit else ['outcome'] if args.outcome else ['smoothing'] if args.smoothing else ['combat'] if args.combat else ['callback'] if args.callback else ['eject'] if args.eject else [f'weapon{args.weapon}'] if args.weapon else ['flight'] if args.flight else [])], cwd=ROOT,
                            capture_output=True, text=True, timeout=180 if args.outcome else 90 if args.combat or args.hit else 45 if args.flight else 25)
    (work / 'native-run.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    exports = [json.loads(line) for line in result.stdout.splitlines()]
    entries = [item for item in exports if 'entry' in item]
    bodies = [item for item in exports if 'capture' in item]
    if args.hit:
        hit_runs=[item for item in exports if item.get('regions')]
        assert len(hit_runs)==1 and (args.gun_approach or hit_runs[0][args.missile+'_hits']>0), exports
        hit_bodies=[item for item in bodies if item.get('hit_body')]
        if not args.gun_approach or hit_bodies:
            assert len(hit_bodies)==1 and hit_bodies[0]['weapon']==hit_weapon, exports
            delta=(hit_bodies[0]['hits_after']-hit_bodies[0]['hits_before'])&0xffff
            assert delta>0 if args.gun else delta==1, exports
        if args.gun_approach:
            gun_samples=[item for item in exports if 'gun_samples' in item]
            assert len(gun_samples)==1 and gun_samples[0]['gun_samples']==32, exports
    if args.kill:
        kills=[item for item in exports if item.get('missile_kill')]
        assert len(kills)==1 and kills[0]['weapon']==hit_weapon and kills[0]['started'] and kills[0]['accounted'] and kills[0]['inactive'], exports
        transitions=[item for item in exports if item.get('kill_transition')]
        start=hit_bodies[0]['body_serial']
        finish=transitions[-1]['body_serial']
        assert not transitions[-1]['flags']&0x40 and finish-start+1==kills[0]['expiry_bodies'], kills
        window={item['body_serial'] for item in bodies if start<=item['body_serial']<=finish}
        assert window==set(range(start,finish+1)), 'Missing body in destruction-to-inactivation interval'
        assert any(item['lifetime']==0 and item['enemy_expiries']==kills[0]['enemy_expiries_before']+1 for item in transitions), transitions
    if args.mode==2:
        wraps=[item for item in exports if item.get('stream_wrap')]
        assert len(wraps)==1 and wraps[0]['wraps']>=1 and wraps[0]['streams']==255 and wraps[0]['returned'], exports
    if args.outcome:
        outcomes=[item for item in exports if item.get('natural_outcome')]
        assert len(outcomes)==1 and outcomes[0]['returned'] and outcomes[0]['failure_seen'] and outcomes[0]['reset_states']==15 and outcomes[0]['aircraft_losses']==3, exports
        transitions=[item for item in exports if item.get('outcome_transition')]
        assert {item['resets_remaining'] for item in transitions} >= {0,1,2,3}, transitions
        restarts=[item for item in exports if item.get('outcome_restart')]
        assert len(restarts)==1 and restarts[0]['mode']==125 and restarts[0]['scene_frames']>=30, exports
    if args.callback:
        assert any(0x46 in item['keys'] for item in entries), entries
        assert any(0xc6 in item['keys'] for item in entries), entries
    if args.smoothing:
        assert any(item['stage']=='C10A24' and not item['keys'] for item in entries), entries
        cancellations=[item for item in exports if item.get('smoothing_cancel')]
        assert len(cancellations)==1 and cancellations[0]['continued'] and cancellations[0]['returned'] and cancellations[0]['scene_frames']>=30, exports
    minimum_entries=44 if args.smoothing else {2:30,3:43,4:39,5:39,6:38,7:42,8:38,125:55}[args.mode]
    minimum_bodies=29 if args.smoothing else {2:21,3:29,4:27,5:27,6:27,7:29,8:27,125:35}[args.mode]
    assert len(entries)>=minimum_entries and len(bodies)>=minimum_bodies, exports
    if args.eject:
        assert len(entries)>=46 and len(bodies)>=43, exports
    if args.weapon:
        assert len(entries)>=42+2*args.weapon and len(bodies)>=(36 if args.weapon==3 else 38), exports
        checkpoints=[item for item in exports if item.get('weapon_checkpoint')]
        assert len(checkpoints)==1, exports
    if args.flight:
        regions=[item for item in exports if item.get('regions')]
        assert len(regions)==1 and regions[0]['spawned_records'] and regions[0]['zone_exits'] and regions[0]['npc_missiles']&(1<<13), exports
    required={'C10DAE'}
    if args.flight:
        required|={'C11788','C11830','C11872'}
    if args.mode==2:
        required|={'C10272','C1029E','C102D8','C10302','C10362','C0F946','C0F974','C0F992'}
    elif args.mode==125:
        required|={'C10272','C1029E','C10C08','C0F992','C0FCB4'}
    else:
        required|={'C103E4','C10418','C10458','C104C2','C105A6','C105F4',
                   'C10626','C1064C','C10900','C10942','C10970','C109AC',
                   'C10A24','C10B1E'}
        if args.mode==3:
            required|={'C10AB2','C10AE6'}
        if args.mode==7:
            required|={'C11078','C110A4'}
        if args.eject:
            required|={'C1104C','C118FC','C11934','C0F920'}
    if args.outcome:
        required|={'C11788','C11830','C11872','C118A0','C118E6','C0F920','C0FBE0'}
    if args.smoothing:
        required.discard('C10B1E')
        required|={'C10C68','C10CFE','C10D8A'}
    assert required <= {
        item['stage'] for item in bodies}, bodies
    (work / 'captures.json').write_text(json.dumps(exports, indent=2) + '\n')
    source_guidance_fault_returns=0
    for name, captures in (('mode_entry', entries), ('frame_body', bodies)):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        # Shared GNU reference objects require sequential builds.
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                        str(oracle.relative_to(ROOT)), '--main',
                        f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        with (work / f'{name}-check.log').open('w') as log:
            for item in captures:
                if name == 'mode_entry':
                    capture = str(prefix) + f".entry.{item['entry']}"
                    values = [str(item['tick']), *map(str, item['keys'])]
                else:
                    capture = str(prefix) + f".{item['capture']}"
                    values = [str(item['before_tick']), str(item['after_tick']),
                              str(item['saved_tick']), capture + '.source.dat']
                    if item['owner_exit']:
                        values.append('owner-exit')
                comparison = subprocess.run([str(oracle), capture + '.before.dat',
                                             capture + '.after.dat', *values], cwd=ROOT,
                                            capture_output=True, text=True, timeout=15)
                log.write(comparison.stdout + comparison.stderr)
                log.flush()
                print(comparison.stdout, end='', flush=True)
                for line in comparison.stdout.splitlines():
                    if line.startswith('Source guidance C06C02 returns: '):
                        source_guidance_fault_returns+=int(line.split(': ')[1])
                if comparison.returncode:
                    retain_failure(capture, work)
                    raise RuntimeError(comparison.stderr or comparison.stdout)
                if item.get('hit_body'):
                    pilot_log=item['pilot_log']
                    log_offset=(pilot_log if pilot_log<0x80000 else 0x80000+pilot_log-0xc00000)+60+4*(0 if item['weapon']==3 else item['weapon'])
                    for suffix, count in (('before',item['hits_before']),
                                          ('after',item['hits_after']),
                                          ('source',item['hits_after'])):
                        ram=Path(capture+f'.{suffix}.dat').read_bytes()
                        if int.from_bytes(ram[log_offset:log_offset+2],'big')!=count:
                            retain_failure(capture, work)
                            raise AssertionError((suffix,item))
                if not args.keep_captures:
                    for suffix in ('before','after','source'):
                        Path(capture+f'.{suffix}.dat').unlink(missing_ok=True)
    if args.combat and args.mode==8:
        assert source_guidance_fault_returns>0, 'No complete original guidance fault/continuation body compared'
    outcome={2:'returns to menu',3:f'runs over 768 scene frames with aircraft {args.aircraft}',4:'runs over 2000 scene frames',5:'runs over 2000 scene frames',6:'runs over 384 scene frames',7:'runs over 2000 scene frames with an unlocked saved pilot',8:'runs over 2000 scene frames with an unlocked saved pilot',
             125:'runs over 2,000 scene frames, including Escape/restart'}[args.mode]
    if args.mode==2:
        outcome='runs all seven control streams, reinitializes the scene at wrap and returns to the menu with Escape'
    if args.eject:
        outcome='runs Shift-E through sound, record clone, lifetime rendering and menu return'
    if args.weapon:
        outcome=f'fires source weapon selection {args.weapon}, preserving ammunition and log counters'
    if args.flight:
        outcome='takes off and runs region spawn/orientation, zone exit, NPC missiles and postflight restart'
    if args.callback:
        outcome='runs Delete callback removal/reinstallation and continues through Escape/restart'
    if args.smoothing:
        outcome='executes a controlled smoothing cancel/reset, resumes active flight and returns to the menu'
    if args.combat:
        outcome='runs sustained throttle/stick/target/fire input and manoeuvre-limit crossings'
    if args.outcome:
        outcome='reaches normal-input failure, exhausts all three aircraft resets, returns to the menu and starts Free Flight'
        report={
            'scenario': 'normal-input-mode-six-failure-and-freeflight-restart',
            'native_state_seeded': False,
            'input_stage_intervals': len(entries), 'sampled_bodies': len(bodies),
            'outcome': outcomes[0], 'restart': restarts[0],
            'test_sha256': hashlib.sha256(args.test.resolve().read_bytes()).hexdigest(),
            'adf_sha256': hashlib.sha256((ROOT/'local/media/fa18.adf').read_bytes()).hexdigest(),
            'reference_scope': 'Original instructions from sampled native before-states; not an independent complete mission replay',
        }
        (work/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    if args.hit:
        outcome=f'registers a normal-input {args.missile} '+('hit' if args.gun else 'missile hit')
        report={
            'scenario':f'normal-input-mode-eight-{args.missile}-hit',
            'native_flight_state_seeded':False,
            'eligibility_fixture':'Saved pilot mission-availability byte only; reopened through normal loader',
            'input_stage_intervals':len(entries), 'sampled_bodies':len(bodies),
            'hit_body':hit_bodies[0] if hit_bodies else None, 'flight':hit_runs[0],
            'test_sha256':hashlib.sha256(args.test.resolve().read_bytes()).hexdigest(),
            'adf_sha256':hashlib.sha256((ROOT/'local/media/fa18.adf').read_bytes()).hexdigest(),
            'reference_scope':'Original instructions from native before-states, including exact hit body; not an independent complete mission or verified kill',
        }
        if args.gun_approach:
            outcome='compares the gun approach and its observed hit/miss result'
            report['scenario']='normal-input-mode-eight-gun-approach'
            report['active_gun_bodies']=gun_samples[0]['gun_samples']
            report['reference_scope']='Original instructions from sampled native before-states, including 32 active-projectile bodies; no gun shoot-down or independent complete mission established'
        (work/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    if args.kill:
        outcome=f'destroys an enemy aircraft with a {args.missile} missile, counts its expiry and observes its inactivation'
        report['scenario']=f'normal-input-mode-eight-{args.missile}-kill'
        report['kill']=kills[0]
        report['kill_transitions']=[item for item in exports if item.get('kill_transition')]
        report['reference_scope']='Original instructions from native before-states, including hit and continuous destruction-to-inactivation bodies; not an independent complete mission'
        (work/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'Mode {args.mode} {outcome}; {len(entries)} actual input/stage intervals and '
          f'{len(bodies)} frame bodies match compared original RAM/display')


if __name__ == '__main__':
    main()
