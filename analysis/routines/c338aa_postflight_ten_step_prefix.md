# `$C338AA` postflight ten-step prefix

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It initializes `$C45B26` to 10, saves `$C45B22` and `d1`, then repeatedly
adds `$A0` to `d1` while the result is at most `$B0`. Each iteration performs
three `ABCD` operations from `$C45B2A` into `$C45B26`, clamps `$C45B22` below
`$360`, selects `$C33A16 + d1`, and calls `$C33F54`.

After restoring the saved values it forces `$C45B22` to `$360` if nonpositive.
`$C3391A` begins the following reverse-step path. The BCD storage and helper
purpose remain unproven.
