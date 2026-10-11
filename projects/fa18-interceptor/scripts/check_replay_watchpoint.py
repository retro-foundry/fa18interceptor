"""Test a filtered CPU write during ordinary full-frame replay."""
import argparse
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


class Watchbreak(C.Structure):
    _fields_ = [(name, C.c_uint32) for name in (
        'index', 'watch_addr', 'op_mask', 'diff_operand', 'value_operand',
        'old_value_operand', 'size_operand', 'addr_mask_operand', 'access_source_operand',
        'access_addr', 'access_kind', 'access_size', 'value', 'old_value',
        'old_value_valid', 'access_source', 'access_source_detail')]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--address', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--access', choices=['read', 'write'], default='write')
    parser.add_argument('--value', type=lambda x: int(x, 0))
    parser.add_argument('--arm-frame', type=int, required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--frame-offset', type=int, default=0,
                        help='Replay-frame label immediately before the first executed frame.')
    parser.add_argument('--expect', choices=['hit', 'miss'], required=True)
    parser.add_argument('--any-source', action='store_true')
    parser.add_argument('--source', choices=['cpu', 'dma', 'blitter', 'copper', 'audio', 'video', 'peripheral', 'disk'])
    parser.add_argument('--address-mask', type=lambda x: int(x, 0), default=0xffffff,
                        help='24-bit address compare mask (default: exact address)')
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    parser.add_argument('--output', type=Path,
                        help='new directory in which to preserve the paused state on a hit')
    args = parser.parse_args()
    if args.any_source and args.source:
        parser.error('--any-source and --source cannot be combined')
    if args.frames < 1 or args.frame_offset < 0:
        parser.error('--frames must be positive and --frame-offset nonnegative')
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), ROOT / 'local/saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')
    add = engine.bind('e9k_debug_add_watchpoint', C.c_int, *([C.c_uint32] * 8))
    consume = engine.bind('e9k_debug_consume_watchbreak', C.c_int, C.POINTER(Watchbreak))
    hit = None
    for local_frame in range(1, args.frames + 1):
        frame = args.frame_offset + local_frame
        if frame == args.arm_frame:
            # e9k-lib.h v0.62-alpha: source 1=CPU, 2=DMA, 3=blitter,
            # 4=copper, 5=audio, 6=video, 7=peripheral, 8=disk.
            op_mask = (1 if args.access == 'read' else 2) | 64
            source_values = {'cpu': 1, 'dma': 2, 'blitter': 3, 'copper': 4,
                             'audio': 5, 'video': 6, 'peripheral': 7, 'disk': 8}
            if not args.any_source:
                op_mask |= 128
            if args.value is not None:
                op_mask |= 8
            source = source_values.get(args.source, 1)
            index = add(args.address, op_mask, 0, args.value or 0, 0, 0,
                        args.address_mask, source)
            if index < 0:
                raise RuntimeError('Could not install watchpoint')
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        watch = Watchbreak()
        if consume(C.byref(watch)):
            registers = engine.regs()
            stack_size = min(256, 0xc80000 - registers['a7'])
            hit = {'frame': frame, 'registers': registers,
                   'instruction_bytes': engine.memory(registers['pc'], 16).hex(),
                   'stack_bytes': engine.memory(registers['a7'], stack_size).hex(),
                   'watch': {name: getattr(watch, name) for name, _ in Watchbreak._fields_}}
            if args.output:
                args.output.mkdir(parents=True, exist_ok=False)
                (args.output / 'state.bin').write_bytes(engine.state())
                hit['paused_state'] = str(args.output / 'state.bin')
            break
    actual = 'hit' if hit else 'miss'
    report = {'address': f'{args.address:06x}', 'address_mask': f'{args.address_mask:06x}',
              'access': args.access, 'value': args.value, 'source': args.source or ('any' if args.any_source else 'cpu'),
              'arm_frame': args.arm_frame,
              'frame_offset': args.frame_offset,
              'expect': args.expect, 'actual': actual, 'hit': hit}
    print(json.dumps(report, indent=2))
    if actual != args.expect:
        raise AssertionError(report)
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
