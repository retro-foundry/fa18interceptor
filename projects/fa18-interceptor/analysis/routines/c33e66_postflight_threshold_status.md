# `$C33E66` postflight threshold status resolution

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It compares the selected record word `$4A(a1)` to the `d7` threshold prepared
by the preceding gate, then tests `$C459C0` and `$C45B46`. Depending on those
values and a masked `$C458DB` byte, it writes enable values 1 or 2 to
`$C458B4` and sets bit 2 or bit 4 in `$C45B54`.

The clear path writes zero to `$C458B4` and branches to `$C33F06`; other paths
converge at `$C33F38`. Status meanings remain unproven.
