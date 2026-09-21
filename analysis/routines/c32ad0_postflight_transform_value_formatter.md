# `$C32AD0` postflight transform value formatter

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this complete `$C32AD0-$C32AFF` prefix.

`source_amiga/observed/format_postflight_transform_value.asm` is byte-exact.
It takes `$C45B22`, emits low nibbles first as ASCII `0`--`?` bytes while
right-shifting the value, then—when `D4` is zero—replaces leading emitted `0`
bytes with spaces. It joins `$C32B00` to consume the `A1` table pairs.

The source value's UI label is not proven. The output is an ASCII-formatted
field consumed in the same transform-submit path as `$C33258`.
