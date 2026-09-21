# `$C31900` segment-39 data boundary

Classification: **confirmed structural data boundary**. The bytes beginning at
`$C31900` are ordered long values (for example `$0000A000`, `$0000E000`, and
`$00025000`), not a valid instruction entry sequence. Existing capture
planning places segment 39 at `$C31900-$C33250`; no P-code function entry in
that segment is currently available.

The next direct postflight code entry reconstructed after this table-heavy
region is `$C332BC`. This note deliberately does not assign a table meaning or
convert its bytes into source instructions until a consumer is traced.
