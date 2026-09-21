# `$C2FD22` renderer work-buffer clear

Classification: **static-only dataflow**. `$C0FA04` directly calls this helper
when its follow-up condition is non-negative, but no available P-code export
enters `$C2FD22`.

`source_amiga/observed/clear_renderer_work_buffers.asm` is byte-exact for the
complete `$C2FD22-$C2FD8B` range (106 bytes). It clears four longword streams
from `$C456BE-$C456CA` for 2,000 iterations and conditionally clears a fifth
stream from `$C456CE` when `$C457D6` is non-zero. It then clears five streams
from `$C456D2-$C456E2` unconditionally for the same iteration count.

The pointer values, buffer contents, and game-level purpose of this cleanup
remain unassigned without runtime evidence.
