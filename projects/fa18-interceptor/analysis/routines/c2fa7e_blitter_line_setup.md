# `$C2FA7E` blitter-line parameter setup

Classification: **byte-exact hardware-facing source**. This prefix of the
known blitter-line emitter derives line direction, row-relative offset,
Bresenham error values, line-mode control bits, and the `BLTSIZE` trigger word
from caller registers. It ends at `$C2FB4E`, where the existing
`blitter_line_setup.asm` waits for the blitter and begins custom-register
writes.

The complete `$C2FA7E-$C2FB4D` range (208 bytes) is
`source_amiga/observed/prepare_blitter_line_parameters.asm`. Coordinate-system
and primitive ownership remain unassigned, but the arithmetic is tied directly
to the documented line-mode hardware sequence.
