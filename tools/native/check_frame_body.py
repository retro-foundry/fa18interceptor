"""Compare actual native frame bodies with original C0EFEA-C0F3C0.

This exercises assembled gameplay/drawing, not original task/display cadence.
No original full replay is repeated. Copper fade is excluded by comparing
drawing planes; host scratch, asynchronous voices and busy counters are named
exclusions in the source oracle.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--carrier-input', type=Path)
    parser.add_argument('--case', action='append', choices=('selection', 'viewport', 'active', 'readout',
                                                          'crash-flight', 'carrier-approach', 'cockpit', 'map'))
    args = parser.parse_args()
    oracle = ROOT / 'build/recomp/native_frame_body_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_frame_body_oracle.c'], cwd=ROOT, check=True)
    scenarios = [('selection', ROOT / 'captures/native/demo01/input.fa18in', 1525),
                 ('viewport', ROOT / 'captures/native/demo01/input.fa18in', 1750),
                 ('active', ROOT / 'captures/native/demo01/input.fa18in', 2401),
                 ('readout', ROOT / 'captures/native/demo01/input.fa18in', 2405),
                 ('crash-flight', ROOT / 'captures/native/qual_fail_crashes/input.fa18in', 2000)]
    if args.carrier_input:
        scenarios.append(('carrier-approach', args.carrier_input.resolve(), 6289))
    with tempfile.TemporaryDirectory(prefix='native-frame-body-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        freeflight = work / 'freeflight.fa18in'
        freeflight.write_text('FA18_GAME_INPUT_V1\nend 3500 0\n')
        cockpit_keys = work / 'cockpit.e9k'
        map_keys = work / 'map.e9k'
        events = ((1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49))
        cockpit_replay = 'E9K_INPUT_V1\n' + ''.join(
            f'F {frame} K {key} 0 0 1\nF {frame + 2} K {key} 0 0 0\n' for frame, key in events)
        cockpit_keys.write_text(cockpit_replay)
        map_keys.write_text(cockpit_replay + 'F 6200 K 77 0 0 1\nF 6202 K 77 0 0 0\n')
        scenarios.extend([('cockpit', freeflight, 3000), ('map', freeflight, 3200)])
        if args.case:
            scenarios = [case for case in scenarios if case[0] in args.case]
            if set(args.case) - {case[0] for case in scenarios}:
                parser.error('carrier-approach requires --carrier-input')
        for name, inputs, iteration in scenarios:
            prefix = work / name
            replay = cockpit_keys if name == 'cockpit' else map_keys if name == 'map' else warmup
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                                     '--input', str(inputs), '--iterations', str(iteration + 1),
                                     '--replay', str(replay), '--save-dir', str(work / f'{name}-pilot'),
                                     '--frame-capture', str(iteration), str(prefix)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
            stats = json.loads(result.stdout)
            assert stats['frame_capture_complete'] and stats['replay_iterations'] == iteration + 1, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            assert stats['frame_before_tick'] <= stats['frame_after_tick'], stats
            if name in ('cockpit', 'map'):
                assert stats['mode'] == 1 and stats['stage'] == 'C10DAE' and stats['hud_frames'] > 0, stats
                before = Path(str(prefix) + '.before.dat').read_bytes()
                assert bool(before[0xC45785 - 0xC00000 + 0x80000]) == (name == 'map'), name
            print(f"{name}: update {iteration}, saved tick {stats['frame_saved_tick']}, "
                  f"PAL {stats['frame_before_tick']}..{stats['frame_after_tick']}", flush=True)
            subprocess.run([str(oracle), str(prefix) + '.before.dat', str(prefix) + '.after.dat',
                            str(stats['frame_before_tick']), str(stats['frame_after_tick']),
                            str(stats['frame_saved_tick']), str(prefix) + '.source.dat'],
                           cwd=ROOT, check=True, timeout=15)
    print(f'{len(scenarios)} actual native frame bodies match original drawing/gameplay; recorded timing parity remains open')


if __name__ == '__main__':
    main()
