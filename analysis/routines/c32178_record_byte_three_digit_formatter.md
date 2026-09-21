# `$C32178`: selected-record byte three-digit formatter

Classification: **runtime-backed formatter dataflow**; the record byte's
gameplay and screen-label semantics remain unassigned.

`source_amiga/observed/format_record_byte_as_three_digits.asm` is byte exact
for `$C32178-$C321D1`.

The stage selects `$C46184 + $C458DE`, reads signed byte `+$2B`, shifts its
magnitude left eight bits, divides it unsigned by `$133`, then calls
`$C31C20` with scratch-adjacent state at `$C458FC`. A negative result from
that helper suppresses drawing. Otherwise it writes the scaled long to
`$C45B1E`, calls the proven `$C25A08` packed-BCD converter, and requests a
three-character font render through `$C3273C`. Its coordinate data begins at
`$C3198C`; the observed parameters are `$1B76`, `$1E`, and `D5=4`.

This is the adjacent normal-flight companion to the four-character
`$C321D2` stage. Together they establish that the selected display record is
a source for multiple live numeric glyph submissions, without assigning either
record offset to a cockpit label from appearance alone.
