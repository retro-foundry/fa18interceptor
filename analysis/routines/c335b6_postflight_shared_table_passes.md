# `$C335B6` postflight shared table passes

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The entry selects a record at `$C46184 + $C458DE`, derives a nonnegative word
from `$6E(a0)` when record bit 7 is clear, and takes the `$C33644` branch when
`$62(a0) == $10`. Otherwise it divides the derived value by 12, publishes it
to `$C45B1E`, and calls `$C25A08`.

It then performs two configured local helper calls: `$C32AA4` with fixed
register/table inputs rooted at `$C33294` and `$D2A`, followed by `$C32AB4`
with inputs rooted at `$C33642`, `$C332A8`, and `$E44`. The final branch goes
to `$C337DC`. The record/table and helper semantics are not yet established.
