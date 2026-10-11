"""Capture direct-core diagnostic probes, not deterministic replay images.

The bridge bypasses Engine9000's host-frame scheduler.  Its PNGs are useful for
CPU/state inspection only and must never be used as visual evidence for a
sealed flight recording; use render_run.py for that.
"""
import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    frame_group = parser.add_mutually_exclusive_group(required=True)
    frame_group.add_argument('--frames', type=int, nargs='+')
    frame_group.add_argument('--frame-range', type=int, nargs=3,
                             metavar=('FIRST', 'LAST', 'STEP'),
                             help='inclusive replay range sampled at STEP frames')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--direct-core', action='store_true',
                        help='acknowledge that this is a diagnostic direct-core capture')
    parser.add_argument('--resume', action='store_true',
                        help='reuse an incomplete output directory and capture only missing PNGs')
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    if not args.direct_core:
        parser.error('direct-core output is not valid replay rendering; use scripts/render_run.py')
    if args.frame_range is not None:
        first, last, step = args.frame_range
        if step < 1 or last < first:
            raise ValueError('frame-range requires FIRST <= LAST and positive STEP')
        frames = list(range(first, last + 1, step))
    else:
        frames = sorted(set(args.frames))
    if not frames or frames[0] < 1:
        raise ValueError('Frame numbers must be positive')
    if args.output.exists() and not args.resume:
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True, exist_ok=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / 'saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')
    records = []
    wanted = set(frames)
    for frame in range(1, frames[-1] + 1):
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        if frame in wanted:
            png = args.output / f'frame_{frame:05d}.png'
            if not png.exists():
                engine.screenshot(png)
            records.append({'frame': frame, 'pc': f"{engine.regs()['pc']:06x}",
                            'image': png.name})
    (args.output / 'keyframes.json').write_text(json.dumps({
        'recording': str(args.playback), 'frames': records
    }, indent=2) + '\n')
    engine.core.retro_unload_game()
    engine.core.retro_deinit()
    print(json.dumps(records))


if __name__ == '__main__':
    main()
