"""Measure native F1--F10 effects from the verified post-F10 run003 state."""
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT

STATE = ROOT / 'build/run003_post_f10_first/state.bin'
CONFIG = ROOT / 'captures/run003/config.uae'


def read_hex(engine, address, size):
    return engine.memory(address, size).hex()


def main():
    rows = []
    state = STATE.read_bytes()
    for key in range(282, 292):
        saves = ROOT / 'build/post_f10_function_key_probe' / str(key) / 'saves'
        saves.mkdir(parents=True, exist_ok=True)
        engine = Engine(CONFIG, saves)
        engine.core.retro_run()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError('Core rejected post-F10 state')
        for frame in range(1, 13):
            if frame == 1:
                engine.event('K', [key, 0, 16, 1])
            if frame == 6:
                engine.event('K', [key, 0, 16, 0])
            engine.core.retro_run()
        rows.append({
            'native_key': key,
            'function_key': f'F{key - 281}',
            'function_level_byte': read_hex(engine, 0xC45870, 1),
            'scaled_control_words': read_hex(engine, 0xC45778, 4),
            'command_pending': read_hex(engine, 0xC457A3, 1),
            'raw_queue': read_hex(engine, 0xC457E1, 10),
        })
        engine.core.retro_unload_game()
        engine.core.retro_deinit()
    report = {'initial_state': str(STATE.relative_to(ROOT)), 'frames_per_probe': 12,
              'events': 'K <native function key> 0 16 press/release', 'rows': rows}
    path = ROOT / 'analysis/post_f10_function_key_probe.json'
    path.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
