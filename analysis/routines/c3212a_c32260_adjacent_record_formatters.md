# `$C3212A` and `$C32260`: adjacent selected-record numeric formatters

Classification: **runtime-backed formatter dataflow**; the record-field and
cockpit-label semantics are unassigned.

Byte-exact source is retained in:

- `source_amiga/observed/format_record_offset72_as_five_digits.asm`
- `source_amiga/observed/format_scaled_record_offset14_as_four_digits.asm`

`$C3212A-$C32177` reads selected-record long `+$72`, shifts it right eight
bits, passes it through `$C31C20`, then converts a non-negative result to
packed BCD at `$C45B22`. It requests five characters through `$C32736`, with
coordinate table `$C31994` and parameters `$1CA4/$0C`.

`$C32260-$C322ED` reads selected-record long `+$14`, applies
`((value - $0F000000) arithmetic_shift_right 8) signed_divide $5999 + $4C4`,
and retains a two-pass redraw counter at `$C4583A` when the value changes. It
then makes two four-character submissions through `$C32736` using `$C31A24`
and `$1D84/$24`, unless `$C457A4` suppresses the conversion.

The normal run029 trace reaches both submission geometries. That proves these
are live display paths, but not which visible instrument each geometry serves.
