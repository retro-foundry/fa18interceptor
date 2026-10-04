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
