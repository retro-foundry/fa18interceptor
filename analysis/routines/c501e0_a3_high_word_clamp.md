# `$C501E0` high-word clamp leaf

Classification: **structural**.

This complete, 50-byte leaf is entered directly from `$C50186` after that
caller supplies `A0` and `A3`. It is observed in 22 independent P-code
captures covering menu, attract, human-flight, and run024 result presentation.

The routine takes the high word of `A3+$08`, imposes a minimum of `$007C`, and
writes it to `A0+$06`. It then takes the high word of `A3+$0C`, masks it with
`$003F`, imposes a maximum from `$C4FF26`, and writes it to `A0+$08`.

`source_amiga/observed/clamp_a3_high_words_to_a0.asm` is byte-exact through
the return. This establishes arithmetic and field offsets only; it does not
assign record, coordinate, display, or gameplay meaning to `A0`, `A3`, or
`$C4FF26`.
