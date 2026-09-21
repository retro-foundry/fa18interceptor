# `$C3391A` postflight reverse-step tail

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It subtracts `$A0` from `d1` while the result is at least `$FF50`. Each pass
performs three `SBCD` operations from `$C45B2A` into `$C45B26`, indexes
`$C33A16 + d1`, and calls `$C33F54`. Its back-edge returns to the packed-value
state test at `$C33908`, rather than directly to the subtraction.

After the loop it loads long `$3D` into `d1`, adds `$C458D8`, calls `$C34068`,
and returns. `$C3395E` begins adjacent data. The BCD storage and helper
semantics remain unproven.
