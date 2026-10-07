"""Compare consecutive independent gameplay boundaries using retained original RAM.

Source captures: FA18_LOOP_DUMP=FIRST+COUNT:PREFIX, at C0EFD4 before input.
Native captures: --frame-capture FIRST+COUNT PREFIX, before stage/input.
Use an established equivalent active-flight boundary and equivalent controls;
the checker never searches for similar frames, seeds state, or reruns the original.
Both complete 320x200 drawing pages are checked, excluding fade colours only.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

from check_gameplay_checkpoint import ROOT, compare_gameplay, integer, span


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--input', type=Path, default=ROOT / 'captures/native/demo01/input.fa18in')
    parser.add_argument('--replay', type=Path, help='host intro/selection keys, default Space at PAL 1800')
    parser.add_argument('--source-prefix', type=Path, required=True)
    parser.add_argument('--source-first', type=int, required=True)
    parser.add_argument('--native-first', type=int, required=True)
    parser.add_argument('--count', type=int, required=True)
    parser.add_argument('--out', type=Path, required=True, help='retain native captures and comparison report here')
    args = parser.parse_args()
    if min(args.source_first, args.native_first, args.count) <= 0:
        parser.error('first iterations and count must be positive')
    if args.count < 2:
        parser.error('a window requires at least two boundaries; use check_gameplay_checkpoint.py for one')
    source_paths = [Path(f'{args.source_prefix}.{args.source_first + i}.dat') for i in range(args.count)]
    for path in source_paths:
        if not path.is_file():
            parser.error(f'missing original boundary: {path}; capture it once before comparing')
    args.out.mkdir(parents=True, exist_ok=True)
    replay = args.replay
    if replay is None:
        replay = args.out / 'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
    prefix = args.out / 'native'
    result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '100000',
                             '--input', str(args.input.resolve()),
                             '--iterations', str(args.native_first + args.count),
                             '--replay', str(replay.resolve()), '--save-dir', str(args.out / 'pilot'),
                             '--frame-capture', f'{args.native_first}+{args.count}', str(prefix.resolve())],
                            cwd=ROOT, check=True, capture_output=True, text=True, timeout=60)
    stats = json.loads(result.stdout)
    assert stats['frame_capture_complete'] and not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
    frames, drawings, motion = [], set(), set()
    for i, path in enumerate(source_paths):
        native_iteration = args.native_first + i
        native_path = Path(f'{prefix}.{native_iteration}.entry.dat')
        source, native = path.read_bytes(), native_path.read_bytes()
        differences = compare_gameplay(source, native)
        planes = b''.join(span(source, integer(source, 0xC4566E + p * 4, 4), 8000) for p in range(8))
        drawings.add(hashlib.sha256(planes).hexdigest())
        motion.add(hashlib.sha256(span(source, 0xC46190, 26)).hexdigest())
        frames.append(dict(source_iteration=args.source_first + i, native_iteration=native_iteration,
                           source_tick=integer(source, 0xC458DA, 2), native_tick=integer(native, 0xC458DA, 2),
                           differences=differences))
    matching = sum(not row['differences'] for row in frames)
    flight_matching = sum(not any(not value.startswith('page ') for value in row['differences']) for row in frames)
    report = dict(source_boundary='C0EFD4/pre-input', native_boundary='C0EFD4/pre-input',
                  compared=args.count, matching=matching, motion_and_controls_matching=flight_matching,
                  distinct_source_drawings=len(drawings),
                  distinct_source_motion=len(motion), frames=frames, native_run=stats)
    report_path = args.out / 'comparison.json'
    report_path.write_text(json.dumps(report, indent=2) + '\n')
    print(f'Gameplay window: {matching}/{args.count} boundaries match ({matching / args.count:.1%}); '
          f'{len(drawings)} distinct drawing states, {len(motion)} distinct motion states')
    print(f'Player motion/pose/matrices, camera, phase and controls: {flight_matching}/{args.count} match')
    for row in [row for row in frames if row['differences']][:3]:
        print(f'Source {row["source_iteration"]}/native {row["native_iteration"]}, '
              f'ticks {row["source_tick"]}/{row["native_tick"]}:')
        for difference in row['differences']:
            print(f'  {difference}')
    print(f'Report: {report_path}; complete recorded sequence acceptance remains separate')
    raise SystemExit(0 if matching == args.count else 1)


if __name__ == '__main__':
    main()
