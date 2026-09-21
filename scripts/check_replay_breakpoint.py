"""Test whether a runtime PC is reached during ordinary full-frame replay."""
import argparse
import ctypes as C
import json
import struct
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--address', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--arm-frame', type=int, default=1,
                        help='Install the breakpoint immediately before this frame')
    parser.add_argument('--expect', choices=['hit', 'miss'], required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), ROOT / 'local/saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')
    hit = None
    for frame in range(1, args.frames + 1):
        if frame == args.arm_frame:
            engine.core.e9k_debug_add_breakpoint(args.address)
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        if engine.core.e9k_debug_is_paused():
            stack = (C.c_uint32 * 64)()
            count = engine.bind('e9k_debug_read_callstack', C.c_size_t, C.POINTER(C.c_uint32), C.c_size_t)(stack, 64)
            regs = engine.regs()
            stack_words = [f'{value:06x}' for value in struct.unpack('>16I', engine.memory(regs['a7'], 64))]
            hit = {'frame': frame, 'registers': regs, 'pc': regs['pc'],
                   'stack_longs_at_a7': stack_words,
                   'callstack': [f'{stack[i]:06x}' for i in range(count)]}
            break
    actual = 'hit' if hit else 'miss'
    report = {'address': f'{args.address:06x}', 'arm_frame': args.arm_frame,
              'expect': args.expect, 'actual': actual, 'hit': hit}
    print(json.dumps(report, indent=2))
    if actual != args.expect:
        raise AssertionError(report)
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
