"""Exercise actual host-clock acquisition and optional visible Free Flight.

PAL replay equality and live host-clock gameplay have distinct clock inputs.
All storage is prepared before gameplay; complete timing rows are retained.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from report_frame_times import report as timing_report

ROOT = Path(__file__).resolve().parents[2]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--visible', action='store_true')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_'))}
    if args.visible:
        assert env.get('SDL_VIDEODRIVER', '') not in ('dummy', 'offscreen')
        assert env.get('SDL_AUDIODRIVER', '') != 'dummy'
    # Existing ordinary-key route from check_clock.py. No aircraft/pilot state
    # is seeded. The visible run adds a final Return to finish its setup.
    events = [(1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49)]
    if args.visible:
        events += [(6100, 13)]
    text = 'E9K_INPUT_V1\n' + ''.join(f'F {frame} K {key} 0 0 1\nF {frame+2} K {key} 0 0 0\n'
                                   for frame, key in events)
    (args.out / 'input.e9k').write_text(text)
    cases = {}
    with tempfile.TemporaryDirectory(prefix='host-clock-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        names = ('visible-host',) if args.visible else ('default-pal', 'explicit-pal', 'explicit-host')
        for name in names:
            folder = work / name
            folder.mkdir()
            command = [str(args.runner.resolve()), '--frames', '6502', '--replay', str((args.out / 'input.e9k').resolve()),
                '--save-dir', str(folder / 'pilot'), '--recorded-input-only', '--data-out', str(folder / 'final.dat'),
                '--clock-report', str((args.out / f'{name}.clock.json').resolve()),
                '--memory-report', str((args.out / f'{name}.memory.json').resolve())]
            if not args.visible:
                command += ['--headless']
            if name.startswith('explicit-'):
                command += ['--clock', name.split('-', 1)[1]]
            if args.visible:
                command += ['--frame-times', str((args.out / 'frames.csv').resolve()),
                            '--flight-trace', str(folder / 'flight.jsonl')]
            result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=260)
            (args.out / f'{name}.log').write_text(result.stdout + result.stderr)
            if (folder / 'final.dat').exists():
                data = (folder / 'final.dat').read_bytes()
                retained = args.out / f'{name}.final.dat.gz'
                retained.write_bytes(gzip.compress(data, mtime=0))
                assert gzip.decompress(retained.read_bytes()) == data
            if (folder / 'flight.jsonl').exists():
                data = (folder / 'flight.jsonl').read_bytes()
                (args.out / f'{name}.flight.jsonl.gz').write_bytes(gzip.compress(data, mtime=0))
            assert result.returncode == 0, (name, result.stderr)
            stats = json.loads(result.stdout)
            clock = json.loads((args.out / f'{name}.clock.json').read_text())
            memory = json.loads((args.out / f'{name}.memory.json').read_text())
            assert stats['frames'] == 6502 and not stats['host_replay_pending'] and not stats['input_queued']
            assert not stats['cpu_emulation'] and not stats['chipset_emulation']
            assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == 0
            assert clock['requests'] > 0
            assert clock['clock'] == ('host' if name.endswith('host') else 'pal')
            if clock['clock'] == 'host':
                assert clock['low_bits_seen'].bit_count() >= 16, 'Host clock lost sub-frame precision'
            else:
                assert clock['low_bits_seen'] == 1
            data = gzip.decompress(retained.read_bytes())
            cases[name] = dict(command=command, stats=stats, clock=clock, memory=memory, final_ram_sha256=sha(data))
        if not args.visible:
            assert cases['default-pal']['stats'] == cases['explicit-pal']['stats']
            assert cases['default-pal']['final_ram_sha256'] == cases['explicit-pal']['final_ram_sha256']
            (args.out / 'explicit-pal.final.dat.gz').unlink()
    evidence = dict(runner_sha256=sha(args.runner.read_bytes()), cases=cases,
        scope='Connected microsecond acquisition and explicit deterministic diagnostics; complete original flight/sound parity remains separate.')
    if args.visible:
        evidence['timing'] = timing_report(args.out / 'frames.csv', 20000)
        (args.out / 'report.json').write_text(json.dumps(evidence, indent=2) + '\n')
        stats = cases['visible-host']['stats']
        assert stats['audio_device'] and stats['scene_frames'] > 0 and stats['hud_frames'] > 0
        assert stats['mode'] == 1 and stats['stage'] == 'C10DAE' and not stats['postflight_resets'], stats
        timing = evidence['timing']['all']
        assert timing['frames'] == timing['presented'] == 6502
        assert not timing['over_work_budget'], 'Visible host-clock gameplay exceeds the 20 ms work budget'
    (args.out / 'report.json').write_text(json.dumps(evidence, indent=2) + '\n')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    print(json.dumps({name: dict(clock=case['clock'], stage=case['stats']['stage']) for name, case in cases.items()}))


if __name__ == '__main__':
    main()
