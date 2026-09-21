# `$C1B27E` control-update trace, frame 5

Classification: **bounded dynamic trace plus structural source**. The sealed
run003 frame-5 trace starts at `$C1B27E` and reaches its nominated return PC
`$C25C76` after 65 instructions, with no future input delivered while stepping.
Canonical P-code is `pcode/raw/run003_c1b27e_control_update/`.

Observed path:

1. `$C459B6` is zero, and `$C4584B` loads as zero, so `$C1B28E` takes the
   signed-less-or-equal branch to `$C1B340`.
2. The low nibble of `$7C(A1)` is zero, `$C457AE` is zero, and `$C45870` is
   `$79`. The routine computes a positive difference of 1 between `$C45870`
   and `$2B(A1)`.
3. The low nibble of `$39(A1)` is 1, selecting the `$C1B3AA` call to
   `$C1B4D0`. The helper writes low-two-bit value 1 at `$C461E9`.
4. With `$C4584B` zero, the trace calls `$C25A6A`, then enters `$C1B410`.
   `$65(A1)` has no bits in masks `$30`, `$C0`, or `$0C`, so the three-axis
   helper stores its cleared `D4` byte to `$28/$29/$2A(A1)` and returns.

This trace proves only this executed branch. The other static branches and
control meanings remain unassigned.
