# `$C33DFE` postflight small variant-delta gate

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It requires `d0 < $DE` and `$2E < d1 < $86`, then requires absolute deltas
from `$C4593A:$C4593E` to be at most 5. Rejected bounds reach `$C33ED8` or
`$C33EE6`.

On success it sets bit `$4000` and clears bit `$200` in `$C45B50`, selects a
threshold `$2700` or `$1800` from `d5`, and otherwise branches to `$C33F52`.
`$C33E66` begins the threshold comparison path. State-bit meaning remains
unproven.
