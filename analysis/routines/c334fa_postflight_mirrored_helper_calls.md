# `$C334FA` postflight mirrored helper calls

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The routine saves `$C45B22`, `d5`, and `d1`; it executes a negative stepping
loop (`d5 -= $258`, `d1 -= $F`) while `d5 >= $AF0`.  Each iteration calls
`$C2F5C0` with `d0 = $DF`, applies three `ABCD` operations from `$C45B2A` to
`$C45B26`, then calls `$C33F8A`.

After restoring its saved state, the second loop only runs when `$C45B22` is
nonzero. It uses the mirrored increments (`d5 += $258`, `d1 += $F`) while
`d5 <= $1130`, applies three `SBCD` operations between the same locations,
and calls `$C33F8A`. The branch at `$C335A0` returns to the second loop's
test at `$C33552`; `$C335A2` is the next routine boundary.

The fixed constants, BCD operations, and helper calls are byte-decoded facts;
their user-visible purpose remains unproven.
