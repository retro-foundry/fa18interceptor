# `$C321D2`: scaled record four-digit formatter

Classification: **runtime-backed formatter dataflow**; record-field semantics
are not assigned.

`source_amiga/observed/format_scaled_record_long_as_four_digits.asm` is byte
exact for `$C321D2-$C3225F`.

The stage selects the current record at `$C46184 + $C458DE`, reads its long at
offset `$1C`, then applies this observed signed calculation:

`((record_long - $10000000) arithmetic_shift_right 8) signed_divide $7000 + $177`

It holds the resulting word at `$C4595C`, retaining a two-pass redraw count at
`$C45839` when it changes. Unless `$C457A4` suppresses conversion, it passes
the sign-extended result through `$C25A08`, producing packed BCD in `$C45B22`.
It then makes two four-character submissions through `$C32736`, using
coordinate data at `$C31A38` and draw parameters `$1C44/$24` followed by the
second mask parameter `$C`.

On the normal run029 trace, this path supplied raw `375` to `$C25A08` and
observed packed output `$00000375`. The trace proves a scaled-record-to-glyph
pipeline, but does not yet tie this particular record field to a legible KTS,
FT, or other cockpit label.
