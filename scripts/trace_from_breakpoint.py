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
    parser.add_argument('--playback', type=Path, required=True)
    parser.add_argument('--address', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--arm-frame', type=int, required=True)
    parser.add_argument('--return-pc', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--max-instructions', type=int, default=10000)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    events = read_events(args.playback)
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
    if future:
        raise ValueError(f'Future input after breakpoint frame {hit_frame}: {future}')
    banks = []
    for name, address, size in [('chip', 0, 0x80000), ('slow', 0xc00000, 0x80000)]:
        data = engine.memory(address, size)
        (args.output / f'{name}.bin').write_bytes(data)
        banks.append({'name': name, 'start': address, 'size': size, 'file': f'{name}.bin', 'sha256': sha(data)})
    (args.output / 'state.bin').write_bytes(engine.state())
    (args.output / 'snapshot.json').write_text(json.dumps({'frame': hit_frame, 'registers': engine.regs(), 'memory': banks,
        'authority': {'initial_state_sha256': sha(args.restore.read_bytes()), 'recording_sha256': sha(args.playback.read_bytes()),
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
    (args.output / 'trace_summary.json').write_text(json.dumps({'breakpoint': f'{args.address:06x}', 'hit_frame': hit_frame,
        'return_pc': f'{args.return_pc:06x}', 'instructions': len(rows),
        'termination': 'return_pc' if reached_return else 'max_instructions',
        'limitation': 'Input was delivered during normal replay before breakpoint; no future input occurred while stepping.'}, indent=2) + '\n')
    engine.core.retro_unload_game()
    engine.core.retro_deinit()
    if not reached_return:
        raise RuntimeError(f'Did not reach return PC {args.return_pc:06x}; retained capped trace')
    print(json.dumps({'hit_frame': hit_frame, 'instructions': len(rows), 'return_pc': f'{args.return_pc:06x}'}))


if __name__ == '__main__':
    main()
