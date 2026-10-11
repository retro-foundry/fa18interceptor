# `$C16F1C` `JOY0DAT` derived-bit update stage

Classification: **structural OCS input stage**.

The complete `$C16F1C-$C16FF3` child is called by `$C16EAE`. It reads OCS
custom register `JOY0DAT` at `$DFF00C`, mirrors the word to `$C45950`, and
first computes `((word & $0200) >> 1) XOR (word & $0100)`. It then separately
tests bit 1 of each byte of the stored big-endian word (word bits 9 and 1).
The nonzero cases call `$C1B510` or `$C1B50C` for the first derived test, and
`$C1B558` or `$C1B55C` for the second; each latches its respective byte at
`$C45831` or `$C45832`.

When a latched test is no longer active, the stage calls `$C1B514` or
`$C1B560` and clears the associated flag. The original code's separate bit
tests and XOR arithmetic are retained exactly.

This proves register access, bit positions, helper edges, and latch mechanics.
It does not assign the derived tests to directions or identify the helpers'
gameplay effects.

## run060 initial-turn instance

The sealed run060 input phase entered this child at frame 942, after the
frame-939 `J 0 5 1` recording event. The first `$0200/$0100` derived test is
zero on this invocation. The following bit-1 XOR test is non-zero and takes
the direct `$C16F88 -> $C1B50C` edge. That helper selects `$20` and the shared
publisher at `$C1B538` writes root control byte `$C461E9` from `$01` to `$21`;
the stage then latches `$C45831 = 1`.

This establishes the executed OCS-derived-bit to root-control-mask path that
precedes the frame-949 pitch-like update. It does not prove which physical
joystick direction the recording code represents.
