# `$C33B8C` postflight variant-pair resolution

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It computes absolute word differences between `$C4593A/$C4593E` and
`$C4593C/$C45940`, requiring both to be at most 8, and also requires
`$4A(a1) <= $900`. The prior gate provides `a1` as the selected record.

When those tests pass and `$C459C0` is nonnegative, the entry sets bit 2 of
`$C45B54`, sets `$C458B4`, and calls `$C31E6C`. The inverse path clears that
byte and sets bit 3 in `$C45B54`. It restores `$C4593E:$C45940` into `d0:d1`,
branches to `$C33CC4` if `d0 <= 0`, otherwise calls `$C2F5C0`.

The state fields, status bits, and helper effects remain unproven.
