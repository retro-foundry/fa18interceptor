"""Replay normally to a breakpoint, then trace a no-future-input instruction interval."""
import argparse
import ctypes as C
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT, sha
from profile_window import read_events


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--playback', type=Path,
                        help='optional E9K input stream; omit when restoring a post-step state')
    parser.add_argument('--address', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--arm-frame', type=int, required=True)
    parser.add_argument('--return-pc', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--max-instructions', type=int, default=10000)
    parser.add_argument('--ignore-future-input', action='store_true',
                        help='trace after the breakpoint without delivering later replay events')
    parser.add_argument('--write-memory', action='append', nargs=3,
                        metavar=('ADDRESS', 'VALUE', 'SIZE'), default=[],
                        help='write VALUE at ADDRESS using SIZE bytes after the breakpoint pauses')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    events = read_events(args.playback) if args.playback else {}
    engine = Engine(args.config.resolve(), args.output / 'saves')
    engine.core.retro_run()
    payload = args.restore.read_bytes()
    if not engine.core.retro_unserialize(payload, len(payload)):
        raise RuntimeError('Core rejected save state')
    hit_frame = None
    for frame in range(1, args.frames + 1):
        if frame == args.arm_frame:
            engine.core.e9k_debug_add_breakpoint(args.address)
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        if engine.core.e9k_debug_is_paused():
            hit_frame = frame
            break
    if hit_frame is None or engine.regs()['pc'] != args.address:
        raise RuntimeError(f'Breakpoint {args.address:06x} not reached')
    future = [frame for frame in events if frame > hit_frame]
    if future and not args.ignore_future_input:
        raise ValueError(f'Future input after breakpoint frame {hit_frame}: {future}')
    writes = []
    write_memory = engine.bind('e9k_debug_write_memory', C.c_int, C.c_uint32,
                               C.c_uint32, C.c_size_t)
    for address_text, value_text, size_text in args.write_memory:
        address, value, size = int(address_text, 0), int(value_text, 0), int(size_text, 0)
        if size not in (1, 2, 4):
            raise ValueError(f'unsupported write size {size}; expected 1, 2, or 4')
        if not write_memory(address, value, size):
            raise RuntimeError(f'debug write failed at {address:06x}')
        writes.append({'address': address, 'value': value, 'size': size})
    banks = []
    for name, address, size in [('chip', 0, 0x80000), ('slow', 0xc00000, 0x80000)]:
        data = engine.memory(address, size)
        (args.output / f'{name}.bin').write_bytes(data)
        banks.append({'name': name, 'start': address, 'size': size, 'file': f'{name}.bin', 'sha256': sha(data)})
    (args.output / 'state.bin').write_bytes(engine.state())
    (args.output / 'snapshot.json').write_text(json.dumps({'frame': hit_frame, 'registers': engine.regs(), 'memory': banks,
        'debug_writes': writes,
        'authority': {'initial_state_sha256': sha(args.restore.read_bytes()),
                      'recording_sha256': sha(args.playback.read_bytes()) if args.playback else None,
                      'snapshot_sha256': sha((args.output / 'state.bin').read_bytes())}}, indent=2) + '\n')
    decoder = capstone.Cs(capstone.CS_ARCH_M68K, capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
    rows = []
    reached_return = False
    for index in range(args.max_instructions):
        regs = engine.regs()
        pc = regs['pc']
        raw = engine.memory(pc, 10)
        ins = next(decoder.disasm(raw, pc, 1), None)
        if ins is None:
            raise RuntimeError(f'68000 decode failed at {pc:06x}')
        row = {'index': index, 'pc': pc, 'bytes': raw[:ins.size].hex(),
               'asm': f'{ins.mnemonic} {ins.op_str}'.strip(), 'registers': regs}
        engine.core.e9k_debug_step_instr()
        engine.core.retro_run()
        row['next_pc'] = engine.regs()['pc']
        rows.append(row)
        if row['next_pc'] == args.return_pc:
            reached_return = True
            break
    (args.output / 'trace.jsonl').write_text(''.join(json.dumps(row, separators=(',', ':')) + '\n' for row in rows))
    input_limitation = ('No replay input was supplied; execution after restore was no-input.'
                        if not args.playback else
                        'Input was delivered during normal replay before breakpoint; '
                        + ('later replay events were deliberately not delivered while stepping.'
                           if future else 'no future input occurred while stepping.'))
    (args.output / 'trace_summary.json').write_text(json.dumps({'breakpoint': f'{args.address:06x}', 'hit_frame': hit_frame,
        'return_pc': f'{args.return_pc:06x}', 'instructions': len(rows),
        'termination': 'return_pc' if reached_return else 'max_instructions',
        'limitation': input_limitation}, indent=2) + '\n')
    final_banks = []
    for name, address, size in [('chip', 0, 0x80000), ('slow', 0xc00000, 0x80000)]:
        data = engine.memory(address, size)
        filename = f'final_{name}.bin'
        (args.output / filename).write_bytes(data)
        final_banks.append({'name': name, 'start': address, 'size': size,
                            'file': filename, 'sha256': sha(data)})
    final_state = engine.state()
    (args.output / 'final_state.bin').write_bytes(final_state)
    (args.output / 'final_snapshot.json').write_text(json.dumps({
        'frame': hit_frame,
        'registers': engine.regs(),
        'memory': final_banks,
        'state_sha256': sha(final_state),
        'stepped_instructions': len(rows),
        'termination': 'return_pc' if reached_return else 'max_instructions',
    }, indent=2) + '\n')
    engine.core.retro_unload_game()
    engine.core.retro_deinit()
    if not reached_return:
        raise RuntimeError(f'Did not reach return PC {args.return_pc:06x}; retained capped trace')
    print(json.dumps({'hit_frame': hit_frame, 'instructions': len(rows), 'return_pc': f'{args.return_pc:06x}'}))


if __name__ == '__main__':
    main()
