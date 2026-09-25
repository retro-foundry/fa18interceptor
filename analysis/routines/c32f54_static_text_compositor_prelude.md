# `$C32F54-$C32FCD`: static-text compositor prelude

Classification: **static formatter/event-layout dataflow**, with a bounded
run060 path observation.

`source_amiga/observed/prepare_static_text_compositor.asm` is byte exact for
`$C32F54-$C32FCD`. It restores `(A1,A2,A4)` from `$C456FE`, prepares the
`$C3D8FC` table and `$C456B6` renderer pointer, then dispatches according to
`$C457E0`, incoming `D4`, and a byte map at `$C331CE`.

For mode at least two, a nonzero mapped value below `$20`, a nonpositive
`$C457DE`, or a zero original `D4` follows external continuation paths. The
remaining path may write the mapped byte through `$C1AB74 + $1E + D5`, advances
`$C457F6`, decrements `$C457F5`, and joins `$C32FD8`. The entry below mode two
joins the established `$C32FCE` glyph setup.

In the run060 frame-9285 native-checkpoint trace, `$C32EE4` branches here
after decrementing `$C457DE`; `$C457E0` is below two, so the trace follows the
direct `$C32FCE` glyph-setup path. This is an observed message-rendering route
inside the text-onset interval, not evidence for a qualification decision or
for the semantic meaning of the event/map values.
