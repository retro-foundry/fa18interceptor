# `$C31612` postflight renderer selection

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation executes a selection path before final submission;
other category branches remain unobserved.

`source_amiga/observed/select_postflight_renderer_submission.asm` reproduces
`$C31612-$C316BF` (174 bytes). It bounds offset coordinates, loads a selected
record, compares a long value, derives a category from record bits and byte
`$62`, writes selector values 1, 2, 4, 5, or 8 to `$C45954`, and restores the
caller’s `a0`.

The selector categories and visible outcome remain unproven.
