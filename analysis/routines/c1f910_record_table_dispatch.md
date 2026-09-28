# `$C1F910-$C1F94D`: record-table dispatch entry

Classification: **bounded indirect selector dispatch**.

`record_table_dispatch.{c,h}` ports the byte-exact observed entry in
`source_amiga/observed/record_table_dispatch_entry.asm`. It reads one signed
A2 word. A nonnegative word takes the externally owned `$C1F94E` next-control
edge. `$FFFF` invokes the required error reporter and takes the external
`$C1F8EC` control edge. Other negative words increment the source record count
when bit `$4000` is set, retain the `$3FFF` selector mask, and call the
required resolver for the `$C1FCE8` target.

Negative callee status takes the external `$C1F7FA` restart edge; otherwise
the returned word ORs into the source status and takes the external
`$C1F90A` continuation. The target table itself and every target's semantics
remain caller-owned. The contract covers all four exits and status/count
updates; no selector target is invented or scheduled by `game.c`.
