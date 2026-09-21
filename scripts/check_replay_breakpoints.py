"""Check several runtime PCs independently during one bounded replay window."""
import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--address', action='append', required=True,
                        type=lambda text: int(text, 0),
                        help='PC to check; may be repeated.')
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--arm-frame', type=int, default=1)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    if args.frames < args.arm_frame or args.arm_frame < 1:
        raise ValueError('Invalid frame window')
    if args.output.exists():
        raise FileExistsError(args.output)

    state = args.restore.read_bytes()
    events = read_events(args.playback)
    results = []
    for address in args.address:
        engine = Engine(args.config.resolve(), ROOT / 'local/saves')
        engine.core.retro_run()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError('Core rejected save state')
        hit = None
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(address)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            if engine.core.e9k_debug_is_paused():
                hit = {'frame': frame, 'registers': engine.regs()}
                break
        results.append({'address': f'{address:06x}', 'hit': hit})
        engine.core.retro_unload_game()
        engine.core.retro_deinit()

    report = {
        'restore': str(args.restore),
        'playback': str(args.playback),
        'arm_frame': args.arm_frame,
        'frames': args.frames,
        'results': results,
    }
    args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'checked': len(results), 'hits': sum(row['hit'] is not None for row in results)}))


if __name__ == '__main__':
    main()
