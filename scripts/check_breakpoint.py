"""One bounded check that the real Engine9000 core stops at an observed call."""
from engine9000_bridge import Engine, ROOT
import json
from pathlib import Path
import time

start = time.monotonic()
e = Engine(ROOT / 'local/fa18.uae', ROOT / 'local/saves')
e.core.retro_run()
data = (ROOT / 'captures/baseline_menu/state.bin').read_bytes()
assert e.core.retro_unserialize(data, len(data))
target = 0xc0efd4  # Structural call target observed in menu_trace02; no semantic name yet.
e.core.e9k_debug_add_breakpoint(target)
for _ in range(60):
    e.core.retro_run()
    if e.core.e9k_debug_is_paused():
        break
assert e.core.e9k_debug_is_paused() and e.regs()['pc'] == target, e.regs()
report = {'breakpoint': hex(target), 'observed_pc': hex(e.regs()['pc']),
          'passed': True, 'wall_seconds': round(time.monotonic()-start, 3)}
print(json.dumps(report))
(ROOT/'analysis/breakpoint_check.json').write_text(json.dumps(report, indent=2)+'\n')
e.core.retro_unload_game()
e.core.retro_deinit()
