"""Sample PCs during a normal full-frame replay window using Engine9000's profiler."""
import argparse
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


def read_events(path):
    events = {}
    lines = path.read_text().splitlines()
    if not lines or lines[0] != 'E9K_INPUT_V1':
        raise ValueError('Expected E9K_INPUT_V1 recording')
    for line in lines[1:]:
        parts = line.split()
        if parts[0] != 'F':
            raise ValueError(line)
        events.setdefault(int(parts[1]), []).append((parts[2], list(map(int, parts[3:]))))
    return events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--first-frame', type=int, required=True)
    parser.add_argument('--last-frame', type=int, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    if args.first_frame < 1 or args.last_frame < args.first_frame:
        raise ValueError('Invalid frame interval')
    args.output.mkdir(parents=True, exist_ok=False)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / 'saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')
    start_profile = engine.bind('e9k_debug_profiler_start', None, C.c_int)
    stop_profile = engine.bind('e9k_debug_profiler_stop', None)
    next_profile = engine.bind('e9k_debug_profiler_stream_next', C.c_size_t, C.c_char_p, C.c_size_t)
    for frame in range(1, args.last_frame + 1):
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        if frame == args.first_frame:
            start_profile(1)
        engine.core.retro_run()
    chunks = []
    buffer = C.create_string_buffer(65536)
    while True:
        size = next_profile(buffer, len(buffer))
        if not size:
            break
        chunks.extend(json.loads(buffer.raw[:size])['hits'])
    stop_profile()
    hits = {int(row['pc'], 16): {'samples': row['samples'], 'cycles': row['cycles']} for row in chunks}
    report = {'first_frame': args.first_frame, 'last_frame': args.last_frame,
              'recording': str(args.playback), 'hits': {f'{pc:06x}': value for pc, value in sorted(hits.items())},
              'count': len(hits)}
    (args.output / 'profile.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'count': len(hits), 'samples': sum(x['samples'] for x in hits.values()),
                      'cycles': sum(x['cycles'] for x in hits.values())}))
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
