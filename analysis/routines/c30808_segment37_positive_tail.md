# `$C30808` segment-37 positive-counter tail

Classification: **static-only dataflow**. It continues the unobserved positive
branch from `$C3076C`; the available packet returns before this slice.

`source_amiga/observed/run_segment37_counter_positive_tail.asm` is byte-exact
for `$C30808-$C308D7` (208 bytes). It bounds a size/limit input, derives
offsets and blitter values, configures the Custom block, and dispatches one
first plus three repeated submissions before returning.

The calculation and hardware setup are preserved without assigning a gameplay
or graphics interpretation.
