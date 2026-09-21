# `$C31612` postflight renderer selection

Classification: **static-only dataflow**. This block precedes the final
postflight renderer submission and is not covered by current P-code exports.

`source_amiga/observed/select_postflight_renderer_submission.asm` reproduces
`$C31612-$C316BF` (174 bytes). It bounds offset coordinates, loads a selected
record, compares a long value, derives a category from record bits and byte
`$62`, writes selector values 1, 2, 4, 5, or 8 to `$C45954`, and restores the
caller’s `a0`.

The selector categories and visible outcome remain unproven.
