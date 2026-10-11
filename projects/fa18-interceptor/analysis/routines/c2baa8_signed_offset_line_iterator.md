# `$C2BAA8`: signed-offset line-list iterator

Classification: **byte-exact shared M-map line renderer helper**.

[`iterate_clipped_signed_offset_lines.asm`](../../source_amiga/observed/iterate_clipped_signed_offset_lines.asm)
reconstructs `$C2BAA8-$C2BAEF`. It reads a byte stream of
`(signed dy, signed dx, unsigned width-minus-one)` records from `A1`; byte
`$80` terminates the list. Each candidate is checked against the logical
`320 x 180` renderer bounds before it invokes `$C2FA7E`.

The `$C2B93E` flight-object marker entry selects the stream at `$C2B91E` and
jumps here. In run003 that produces the three black map segments. The helper
is generic: its reconstruction proves the record grammar and clipping route,
not the identity or ownership of every line-list caller.
