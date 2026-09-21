"""Save screenshots and lightweight state metadata at chosen replay frames."""
import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--frames', type=int, nargs='+', required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    frames = sorted(set(args.frames))
    if not frames or frames[0] < 1:
        raise ValueError('Frame numbers must be positive')
    args.output.mkdir(parents=True, exist_ok=False)
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
