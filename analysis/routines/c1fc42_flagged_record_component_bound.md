# `$C1FC42`: flagged record-component bound comparison

Classification: **runtime-backed record-walker predicate**.

`source_amiga/observed/compare_flagged_record_component_bound.asm` is byte
exact for `$C1FC42-$C1FCDD`.

## Contract

The helper derives one of three comparison variants from bits `$0C00` of `D7`.
Each reads a shifted word from the record addressed by `$C45A32 + D0`, combines
it with a shifted workspace component, and compares it with the negated
corresponding `$C45A72/$C45A76/$C45A78` value. The long variant sign-extends
the record word before comparison.

When the comparison is below bound, bit 12 of the original `D7` controls
whether the helper writes `D7=1` or clears `D7`; otherwise it returns after a
`BTST`. Callers must consume its condition codes rather than treating `D7`
alone as its result.

## Runtime anchor

The run031 frame-12,000 Golden Gate no-input trace reaches this helper twice
from the `$C1F77C` record-walker branch before dispatching the counted
`$C211DC` projection-record path. It is a live gate for that scene's record
stream, but not a Golden Gate-specific routine.
