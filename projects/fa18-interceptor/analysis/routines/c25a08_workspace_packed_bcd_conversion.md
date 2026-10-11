# `$C25A08`: workspace long to packed-BCD conversion

Classification: **runtime-backed dataflow**.

`source_amiga/observed/convert_workspace_long_to_packed_bcd.asm` is byte
exact for `$C25A08-$C25A3D`.

The helper reads a raw unsigned long from `$C45B1E`, walks the longword
decimal-place table beginning at `$C25A3E`, and writes a packed-decimal result
to `$C45B22`. It retains the input workspace and preserves `D5-D7/A6` around
the calculation.

In the normal run029 renderer trace, `$C32220` sign-extends raw `D0=375` into
`$C45B1E` and calls this helper. The observed final store at `$C25A32` is
`D7=$00000375`. This proves the formatter workspace conversion contract; it
does not yet assign that particular caller to a readable cockpit field.
