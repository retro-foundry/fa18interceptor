"""Replay to a PC, then step until a selected RAM value changes or a boundary is reached."""
import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--breakpoint', type=lambda text: int(text, 0), required=True)
    parser.add_argument('--arm-frame', type=int, required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--address', type=lambda text: int(text, 0), required=True)
    parser.add_argument('--size', type=int, default=2)
    parser.add_argument('--max-instructions', type=int, default=10000)
    parser.add_argument('--stop-pc', type=lambda text: int(text, 0))
    parser.add_argument('--allow-future-input', action='store_true',
                        help='permit a breakpoint before later recorded events; they are not delivered while stepping')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    if args.size not in (1, 2, 4) or args.output.exists():
        raise ValueError('Size must be 1, 2, or 4 and output must not exist')

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), ROOT / 'local/saves')
    engine.core.retro_run()
    state = args.restore.read_bytes()
    if not engine.core.retro_unserialize(state, len(state)):
        raise RuntimeError('Core rejected save state')
    hit_frame = None
    for frame in range(1, args.frames + 1):
        if frame == args.arm_frame:
            engine.core.e9k_debug_add_breakpoint(args.breakpoint)
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        if engine.core.e9k_debug_is_paused():
            hit_frame = frame
            break
    if hit_frame is None or engine.regs()['pc'] != args.breakpoint:
        raise RuntimeError('Breakpoint not reached')
    if not args.allow_future_input and any(frame > hit_frame for frame in events):
        raise ValueError('Future input after breakpoint')

    before = engine.memory(args.address, args.size)
    result = None
    for index in range(args.max_instructions):
        registers_before = engine.regs()
        pc = registers_before['pc']
        raw = engine.memory(pc, 10)
        engine.core.e9k_debug_step_instr()
        engine.core.retro_run()
        after = engine.memory(args.address, args.size)
        if after != before:
            result = {'kind': 'transition', 'instruction': index, 'pc': f'{pc:06x}',
                      'raw': raw.hex(), 'before': before.hex(), 'after': after.hex(),
                      'next_pc': f'{engine.regs()["pc"]:06x}',
                      'registers_before': registers_before,
                      'registers_after': engine.regs()}
            break
        if args.stop_pc is not None and engine.regs()['pc'] == args.stop_pc:
            result = {'kind': 'stop_pc', 'instruction': index, 'pc': f'{args.stop_pc:06x}',
                      'value': after.hex()}
            break
        before = after
    if result is None:
        result = {'kind': 'limit', 'instruction': args.max_instructions,
                  'pc': f'{engine.regs()["pc"]:06x}', 'value': before.hex()}
    report = {'breakpoint': f'{args.breakpoint:06x}', 'hit_frame': hit_frame,
              'address': f'{args.address:06x}', 'size': args.size,
              'max_instructions': args.max_instructions, 'result': result,
              'limitation': 'Input was delivered in normal replay before breakpoint; no future input was stepped.',
              'future_input_permitted': args.allow_future_input}
    args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report))
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
