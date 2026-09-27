"""Capture selected Engine9000 DMA records for one deterministic replay frame."""
from __future__ import annotations
import ctypes as C
import json
from pathlib import Path
from engine9000_bridge import Engine, ROOT
from profile_window import read_events

class Info(C.Structure):
    _fields_ = [('version', C.c_uint32), ('frameSelect', C.c_uint32),
                ('frameNumber', C.c_int32), ('recordToggle', C.c_int32),
                ('hposCount', C.c_int32), ('vposCount', C.c_int32),
                ('dmaHoffset', C.c_int32), ('recordCount', C.c_uint32),
                ('debugDmaEnabled', C.c_uint32)]

class View(C.Structure):
    _fields_ = [('records', C.c_void_p), ('info', Info),
                ('visibleWidth', C.c_int32), ('visibleHeight', C.c_int32),
                ('renderWidth', C.c_int32), ('renderHeight', C.c_int32),
                ('visibleOffsetX', C.c_int32), ('visibleOffsetY', C.c_int32),
                ('dhposWrap', C.c_int32), ('dhposScale', C.c_int32)]

class Record(C.Structure):
    _fields_ = [('hpos', C.c_int), ('vpos', C.c_int), ('dhpos', C.c_int),
                ('dhpos_abs', C.c_int), ('reg', C.c_uint16), ('dat', C.c_uint64),
                ('size', C.c_uint16), ('addr', C.c_uint32), ('evt', C.c_uint32),
                ('evt2', C.c_uint32), ('evtdata', C.c_uint32),
                ('evtdataset', C.c_bool), ('type', C.c_int16), ('extra', C.c_uint16),
                ('intlev', C.c_int8), ('ipl', C.c_int8), ('ipl2', C.c_int8),
                ('cf_reg', C.c_uint16), ('cf_dat', C.c_uint16), ('cf_addr', C.c_uint16),
                ('ciareg', C.c_int), ('ciamask', C.c_int), ('ciarw', C.c_bool),
                ('ciaphase', C.c_int), ('ciavalue', C.c_uint16), ('end', C.c_bool)]

def main() -> None:
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument('--restore', type=Path, required=True)
    p.add_argument('--playback', type=Path, required=True)
    p.add_argument('--frame', type=int, required=True)
    p.add_argument('--playback-frame-offset', type=int, default=0,
                   help='global replay frame represented by restored frame zero')
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--narrow', action='store_true',
                   help='export only blitter registers and active page records')
    p.add_argument('--address-start', type=lambda text: int(text, 0),
                   help='include DMA records at or above this 24-bit address')
    p.add_argument('--address-end', type=lambda text: int(text, 0),
                   help='include DMA records below this 24-bit address')
    args = p.parse_args()
    if (args.address_start is None) != (args.address_end is None) or \
       (args.address_start is not None and args.address_start >= args.address_end):
        p.error('--address-start and --address-end must be supplied as one nonempty range')
    events = read_events(args.playback)
    engine = Engine((ROOT / 'local/fa18.uae').resolve(), args.output.parent / 'saves')
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError('Core rejected save state')
        get_addr = engine.bind('e9k_debug_amiga_get_dma_addr', C.POINTER(C.c_int))
        get_view = engine.bind('e9k_debug_amiga_dma_debug_get_frame_view', C.POINTER(View), C.c_uint)
        get_addr().contents.value = 6  # collect only; avoid debugger display mode
        for frame in range(1, args.frame + 1):
            global_frame = args.playback_frame_offset + frame
            for kind, values in events.get(global_frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
        view = get_view(0)
        if not view:
            raise RuntimeError('DMA frame view unavailable')
        info = view.contents.info
        if C.sizeof(Record) != 88:
            raise RuntimeError(f'unexpected raw record size {C.sizeof(Record)}')
        rows = C.cast(view.contents.records, C.POINTER(Record * info.recordCount)).contents
        selected = []
        for i, r in enumerate(rows):
            requested_address_match = (args.address_start is not None and
                                       args.address_start <= r.addr < args.address_end)
            narrow_match = (requested_address_match or
                            (0xC304B0 <= r.addr <= 0xC30500) or
                            (0xDFF040 <= r.addr <= 0xDFF076) or
                            (0x12BC0 <= r.addr < 0x1A8C0) or
                            r.addr in (0x76EE, 0x14266, 0x12BC0, 0x10026, 0x37))
            if narrow_match or (not args.narrow and r.type != 0):
                selected.append({'index': i, 'hpos': r.hpos, 'vpos': r.vpos, 'reg': f'{r.reg:04X}',
                                 'dat': f'{r.dat:X}', 'size': r.size, 'addr': f'{r.addr:06X}',
                                 'evt': r.evt, 'evt2': r.evt2, 'evtdata': r.evtdata, 'cf_reg': f'{r.cf_reg:04X}',
                                 'cf_dat': f'{r.cf_dat:X}', 'cf_addr': f'{r.cf_addr:06X}',
                                 'type': r.type, 'extra': r.extra})
        report = {'frame_requested': args.frame,
                  'playback_frame_offset': args.playback_frame_offset,
                  'global_frame': args.playback_frame_offset + args.frame,
                  'frame_number': info.frameNumber,
                  'record_count': info.recordCount, 'record_size': C.sizeof(Record),
                  'dma_mode': 6, 'visible': [view.contents.visibleWidth, view.contents.visibleHeight],
                  'address_window': ([args.address_start, args.address_end]
                                     if args.address_start is not None else None),
                  'selected_records': selected}
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
        print(json.dumps({'frame': info.frameNumber, 'records': info.recordCount, 'selected': len(selected)}))
    finally:
        engine.core.retro_unload_game(); engine.core.retro_deinit()

if __name__ == '__main__':
    main()
