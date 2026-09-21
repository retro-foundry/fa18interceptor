# `$C0D752-$C0D7DF`: display-record preparation prefix

Authority: the return-bounded attract cockpit trace at
`pcode/raw/attract_cockpit_c2fede/`. `$C2FEDE` calls `$C0D752`, which returns
to `$C2FEF2`; this prefix executes before its `$C2E758` child.

The prefix takes five consecutive two-word inputs from `$C0D720`, combines each
with a shifted part of `$C45A66` and three-word groups at `$C45BD8`, and writes
three rounded shifted products into five records beginning at `$C4B390` with a
`$1A`-byte stride. It then clears four longwords at `$C4E854` and calls
`$C2E758`.

This establishes arithmetic/dataflow only. The source values, matrix role,
output records, scratch block, and display semantics remain unassigned.
