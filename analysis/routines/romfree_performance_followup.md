# ROM-free performance work

The user prioritizes a faster, smoother game over exact AmigaOS service timing.
The host compatibility milestone now reaches splash, credits, keyboard input, main-menu access
and demo rendering. Exact follow-up is recorded separately in
`romfree_exact_followup.md`.

A first local GNU headless comparison, using the same ADF and keyboard-to-demo
input for 2,600 frames, took 6.593 seconds with `--ports off` and 7.103 seconds
with `--ports on`. Both had zero forbidden accesses and unsupported services.
This is one smoke measurement, not a stable benchmark or a speedup claim. It
does show that enabling all instruction-timed handwritten adapters is not yet
an automatic performance improvement. Keep the current default until profiling
supports changing it. Most startup resources and game rules still execute their
original code, and the machine runs the original PAL timeline.

Next performance work should distinguish host execution throughput from fresh
3D frames and presentation cadence. Headless execution exceeding real time
does not raise the original scene-rendering rate. Measure time spent in input,
game update/rendering, chipset work and SDL presentation, including long frames.
The ROM-free SDL loop now defaults to 50 Hz host pacing without monitor vsync,
avoiding a second blocking presentation clock. `--vsync on` remains available
for users who prefer synchronized presentation. The reference runner retains
its vsync default. Measure cadence on the actual display; this change has not
yet demonstrated smoother flight and does not raise scene-rendering rate.
A deliberate faster simulation
or renderer path may relax original CPU/bus timing, while preserving game rules,
assets, input responsiveness and save formats. Do not claim smoother flight
from startup smoke tests alone.

2026-10-04: `--window --frame-times PATH.csv` now records each frame's input,
combined CPU/chipset simulation, RGB conversion, SDL presentation, pacing wait,
total duration and RGB444 screen-change flag. Window fixture tests verify all
rows and durations, full RAM/CPU/pixel identity against equivalent headless
execution, frame limits, close and file failures. Screen-change counts include
UI/HUD activity and do not establish fresh 3D scene rates. Optional profiling
includes its comparison/copy overhead in presentation cost; CSV writes occur
after the timed interval. Real-monitor cadence still needs measurement.

A 3,600-frame MSVC dummy-display capture selecting free flight at frame 2,200
averaged 2.346 ms simulation and 0.586 ms presentation, with 19.991 ms total
including pacing. Simulation maximum was 8.346 ms. The final 800 frames had
no screen changes and callback C1075A, so this is a menu/launch-preparation
measurement, not active-flight smoothness evidence. Raw ignored artifacts:
`build/amiga/window-freeflight.{csv,ram}`. Do not interpret a selected mode
byte or a high blit count alone as evidence of active 3D flight.

Ordinary game dispatches previously scanned/validated all enabled OS services.
The adapter now caches enabled service bounds whenever the registry changes;
out-of-range PCs skip the scan. Neutral service execution and machine cycles
are unchanged. Adapter fixtures cover arbitrary RAM services, range endpoints,
replacement, failed replacement, enable/disable and reset. Three alternating
GNU before/after runs over the same 3,600-frame frontend replay had medians
6.441 s and 6.359 s (about 1.3% difference, within observed run variation).
All final RAM/CPU, every RGB444 frame and diagnostic counters matched exactly.
This removes unnecessary lookup work but does not demonstrate a meaningful
speedup. Raw measurements: `build/amiga/dispatch-performance.json`.

Next: record actual flight rather than its C1075A launch-preparation screen,
then separate original scene-update cadence from host pacing. Select a faster
simulation/rendering path based on those measurements, with exact service
timing explicitly deferred. Do not simply accelerate gameplay timers.

Active flight now has a repeatable functional checkpoint. The original free-
flight UI needs starting-location and aircraft selections after confirmation;
the former mode-only probe omitted them. `check_romfree_game_paths.py --flight`
verifies C10DAE, CONTEXT_STARTED=1 and PAUSE_A=0 at frames 4,900 and 5,300,
with different original player coordinates and cockpit pixels on GNU/MSVC.

`--fast-forward N` skips host pacing/presentation for the first N frames of a
window run while executing every game/chipset frame and replay event. It makes
startup and active-flight profiling quicker, without changing subsequent game
speed. The window fixture verifies final CPU/RAM/pixels against headless, CSV
frame numbering, frame limits, close and malformed arguments.

MSVC dummy-display qualification profile, fast-forward 4,500 and end at 4,650:
150 presented frames; mean simulation 3.550 ms (p95 6.758, max 13.458), mean
presentation 0.552 ms, mean paced total 19.988 ms. The screen changed 28 times;
final C10DAE/active/unpaused state and zero ROM/fault counters confirm flight.
Raw ignored artifacts: `build/amiga/qualification-flight.{csv,ram}`. This is
an active scene/cockpit sample, not real-monitor cadence or an improved 3D rate.
The host has spare frame budget while guest frame timing still limits updates.
