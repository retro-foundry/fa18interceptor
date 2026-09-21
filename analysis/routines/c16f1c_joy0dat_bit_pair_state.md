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
