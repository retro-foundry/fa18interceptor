# `$C33B36` postflight variant gate

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The entry returns unless `$C45A42 == $80`. It selects a record at
`$C46184 + $C458DE`, masks the high nibble of record byte `$63`, and branches
to `$C33B8C` when that nibble equals `$10`.

Otherwise it branches to `$C33CC4` if `$C459C0` is negative; otherwise it
loads `d0 = $9F`, `d1 = $5B`, tests `$C4593A`, and can reach `$C33C18` when
`$C459C0` is nonnegative. `$C33B80` is the fall-through continuation. State
field meanings are not established.

That continuation stores `d0:d1` as two words at `$C4593E` and transfers to
`$C33C18`; it is reconstructed separately as `$C33B80–$C33B8B`.
