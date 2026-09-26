# `$C2FD22` renderer work-buffer clear

Classification: **static source with a run075 entry**. `$C0FA04` directly
calls this helper when its follow-up condition is non-negative. The bounded
run075 trace `build/port_run075_c0fa04_500_settle1/` enters it at frame 291
with delay 4 and executes its four-stream clear loop.

`source_amiga/observed/clear_renderer_work_buffers.asm` is byte-exact for the
complete `$C2FD22-$C2FD8B` range (106 bytes). It clears four longword streams
from `$C456BE-$C456CA` for 2,000 iterations and conditionally clears a fifth
stream from `$C456CE` when `$C457D6` is non-zero. It then clears five streams
from `$C456D2-$C456E2` unconditionally for the same iteration count.

The pointer values, buffer contents, and game-level purpose of this cleanup
remain unassigned without runtime evidence.

For the native visual model, `fa18_clear_renderer_work_buffer` maps the
proved four-plane zero clear to `FA18IndexedFrameBuffer` index zero. The
run075 controller invokes it on each nonexpired `$C0FA04` tick. This is a
direct chunky work-buffer operation; it does not assert which original pointer
family is currently Copper-visible or reproduce the unrelated extra streams.
