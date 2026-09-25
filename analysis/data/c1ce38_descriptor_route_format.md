# `$C1CE38` normal follow-up descriptor format

The normal `$C1CCBC` route reads a variable-length descriptor from
`$C4F6CA + $C459B0`. Its proven prefix is a header word, handler pointer,
three-word tuple, and long work field.

The header's low nibble is published as shift `$C45AB8`; its high byte becomes
`$C459B4`, and its low byte also becomes selector `$C4585B`. The handler
pointer is guarded against a negative first word and a special `$C1ED48`
case. The tuple is loaded into `D2-D4`, while the following long is published
to `$C45932`; `$C1D0B6` consumes the header and tuple immediately afterward.

This is a descriptor data-layout contract, not a gameplay identity for the
handler or record. Authority: byte-exact
`source_amiga/observed/prepare_alternate_flight_record.asm` and normal run060
trace `build/run060_c1ccbc_normal_full/trace.jsonl`.
