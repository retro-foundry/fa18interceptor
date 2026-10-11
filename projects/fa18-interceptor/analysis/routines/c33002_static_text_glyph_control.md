# `$C33002-$C33057`: static-text glyph control prefix

Classification: **static text/event control dataflow**, with run060 entry
evidence.

`source_amiga/observed/dispatch_static_text_glyph_control.asm` is byte exact
for `$C33002-$C33057`. It converts the pending character in `D4` from ASCII to
a `$20`-based glyph index, examines bits in `$C457DC`, and either joins glyph
rendering at `$C33058`, saves an advanced layout triplet then re-enters
`$C32EF6`, or calls `$C3316A` with `D1=2` before glyph rendering.

The run060 native frame-9285 checkpoint trace reaches this prefix through
`$C32FCE`; its character is nonzero and non-8, so it enters at `$C33002`.
The trace reaches the following `$C33058` glyph-rendering lane. The attributes,
event-control byte, helper purpose, and character source are not promoted into
gameplay semantics.
