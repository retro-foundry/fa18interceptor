# `$C33642` postflight alternate scaled-helper setup

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

`$C33642` is the two-byte value `$4B54`, passed by address to `$C32AB4` from
the preceding table-pass path. `$C33644` is the alternate path selected when
the record byte at `$62(a0)` equals `$10`.

That path divides its incoming value by 12, updates `$C45B1E`, and uses
`$C25A08`, `$C259C2`, and `$C45B22` in the same packed-value pattern seen at
the preceding branch. It derives a scaled `d5`, saves it across `$C33F70`,
and writes 5 to `$C45B26`. The next instruction boundary is `$C336FA`.

The table word, packed representation, helper effects, and user-visible
meaning remain unproven.
