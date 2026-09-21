# `$C33ED8` postflight rejected variant resolution

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The small-value entry calls `$C33DA4`, clears `$C458B4`, and clears `d4`. The
delta-reject entry also clears those values, then conditionally clears bit
`$4000`, sets bit `$200` in `$C45B50`, and sets bit `$100` in `$C45B54` based
on the status bits and masked `$C458DB`.

All paths select `$A` at `$C45954`, call `$C348B2`, and call `$C31E6C` only
when `$C458B4` is nonzero. It returns at `$C33F52`. Status meaning is
unproven.
