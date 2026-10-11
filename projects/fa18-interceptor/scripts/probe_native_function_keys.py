"""Report raw Amiga key bytes produced by native Engine9000 F1--F10 events."""
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT

STATE = ROOT / 'build/free_flight_600/state.bin'
CONFIG = ROOT / 'local/fa18.uae'
KEYBOARD_DISPATCH = 0xC1AD74


def main():
    rows = []
    for key in range(282, 292):
        output = ROOT / 'build/native_function_key_probe' / str(key) / 'saves'
        output.mkdir(parents=True, exist_ok=True)
        engine = Engine(CONFIG, output)
        engine.core.retro_run()
        payload = STATE.read_bytes()
        if not engine.core.retro_unserialize(payload, len(payload)):
            raise RuntimeError('Core rejected in-flight state')
        engine.core.e9k_debug_add_breakpoint(KEYBOARD_DISPATCH)
        engine.event('K', [key, 0, 0, 1])
        hit = None
        for frame in range(1, 9):
            engine.core.retro_run()
            if engine.core.e9k_debug_is_paused():
                regs = engine.regs()
                hit = {'frame': frame, 'pc': f'${regs["pc"]:06X}',
                       'raw_amiga_key': regs['d0'] & 0xff}
                break
        rows.append({'native_key': key, 'function_key': f'F{key - 281}',
                     'keyboard_dispatch': hit if hit and hit['pc'] == '$C1AD74' else None,
                     'non_dispatch_stop': hit if hit and hit['pc'] != '$C1AD74' else None})
        engine.core.retro_unload_game()
        engine.core.retro_deinit()
    report = {'initial_state': str(STATE.relative_to(ROOT)), 'dispatch': f'${KEYBOARD_DISPATCH:06X}',
              'rows': rows}
    target = ROOT / 'analysis/native_function_key_probe.json'
    target.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
