# `$C32EF6-$C32F5B`: message-event layout preparation

Classification: **static message-event/layout dataflow**.

`source_amiga/observed/prepare_message_event_layout.asm` is byte exact for
`$C32EF6-$C32F5B`.  It copies `$C457DB` into the word at `$C45748`, initializes
`$C457DE` to one, and normally joins `$C32F54`.  When incoming `D4` equals
`$41` and `$C457F6` is positive, it decrements that cursor, increments
`$C457F5`, clears one byte through pointer `$C1AB74 + $1E + D5`, clears two
adjacent bytes starting at `$C457EB + D5`, restores `(A1,A2,A4)` from
`$C456FE`, subtracts four from `A1`, and joins `$C32F5C`.

This establishes another writer of the run060-onset candidate `$C457DE`, but
not its input-event meaning.  The `$41` code, external pointer, buffer, and
text layout cannot be promoted to gameplay semantics without a native trace
through this branch.
