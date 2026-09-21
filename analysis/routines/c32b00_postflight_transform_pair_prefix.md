# `$C32B00` postflight transform-pair prefix

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this complete `$C32B00-$C32B3F` prefix.

`source_amiga/observed/prepare_postflight_transform_pair.asm` is byte-exact.
It reads two words from `A1` and a byte from `A2`, rejects a space byte or a
derived row outside `[0,$28)`, adds the prepared offsets, and turns the byte
into a zero-based word-table index by subtracting `$20`.

The following code uses that index to select a table entry and submit to active
renderer lanes. The pair fields' units and visual object remain unproven.
