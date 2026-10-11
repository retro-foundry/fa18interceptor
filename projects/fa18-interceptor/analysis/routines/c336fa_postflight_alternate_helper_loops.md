# `$C336FA` postflight alternate helper loops

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It initializes `$C45B26` to 5 and saves `$C45B22` plus `d5`. The first loop
steps `d5` down by `$258` while it remains at least `$AF0`; each iteration
applies three `ABCD` operations from `$C45B2A` to `$C45B26`, then calls
`$C33F70` with `d5` saved on the stack.

After restoring state, a second loop runs only while `$C45B22` is nonzero. It
steps `d5` upward by `$258` while it stays at most `$1130`, applies three
`SBCD` operations, and calls the same helper. `$C3377A` is the next routine
boundary. No display or gameplay meaning is inferred from this dataflow.
