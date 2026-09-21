"""Sample selected runtime words after matching input frames in a normal replay."""
import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def read_word(engine, address):
    return int.from_bytes(engine.memory(address, 2), 'big', signed=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--word', action='append',
                        type=lambda text: int(text, 0),
                        help='Runtime word address; may be repeated.')
    parser.add_argument('--word-range', action='append', nargs=2,
                        metavar=('START', 'END'), default=[],
                        type=lambda text: int(text, 0),
                        help='Inclusive even-address word interval; may be repeated.')
    parser.add_argument('--input-kind', default='J',
                        help='Only sample frames containing this recording event kind.')
    parser.add_argument('--sample-every', type=int,
                        help='Also sample every N frames in the selected interval.')
    parser.add_argument('--sample-first', type=int, default=1,
                        help='First frame eligible for --sample-every.')
    parser.add_argument('--sample-last', type=int,
                        help='Last frame eligible for --sample-every (default: --frames).')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    words = list(args.word or [])
    for start, end in args.word_range:
        if end < start or start % 2 or end % 2:
            raise ValueError('Word ranges must be ordered even addresses')
        words.extend(range(start, end + 1, 2))
    words = list(dict.fromkeys(words))
    if not words:
        raise ValueError('At least one --word or --word-range is required')
    if args.frames < 1:
        raise ValueError('--frames must be positive')
    if args.sample_every is not None and args.sample_every < 1:
        raise ValueError('--sample-every must be positive')
    sample_last = args.frames if args.sample_last is None else args.sample_last
    if args.sample_first < 1 or sample_last < args.sample_first or sample_last > args.frames:
        raise ValueError('Invalid --sample-first/--sample-last interval')
    if args.output.exists():
        raise FileExistsError(args.output)

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output.parent / 'sample_replay_memory_saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')

    records = []
    for frame in range(1, args.frames + 1):
        frame_events = events.get(frame, [])
        for kind, values in frame_events:
            engine.event(kind, values)
        engine.core.retro_run()
        matching = [(kind, values) for kind, values in frame_events if kind == args.input_kind]
        periodic = (args.sample_every is not None and args.sample_first <= frame <= sample_last
                    and (frame - args.sample_first) % args.sample_every == 0)
        if matching or periodic:
            records.append({
                'frame': frame,
                'events': matching,
                'words': {f'{address:06x}': read_word(engine, address) for address in words},
                'pc_after_frame': f'{engine.regs()["pc"]:06x}',
            })

    report = {
        'restore': str(args.restore),
        'playback': str(args.playback),
        'frames': args.frames,
        'input_kind': args.input_kind,
        'periodic_interval': ([args.sample_first, sample_last, args.sample_every]
                              if args.sample_every is not None else None),
        'word_addresses': [f'{address:06x}' for address in words],
        'records': records,
    }
    args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'records': len(records), 'frames': args.frames}))
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
