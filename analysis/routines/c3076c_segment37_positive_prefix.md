# `$C3076C` segment-37 positive-counter prefix

Classification: **static-only dataflow**. The available `$C30764` packet takes
the preceding non-positive return; this positive path has not been executed.

`source_amiga/observed/run_segment37_counter_positive_prefix.asm` is
byte-exact for `$C3076C-$C30807` (156 bytes). It decrements `$C45836`, derives
pointer and blitter inputs, invokes external helpers, writes setup values to
the Custom block, then makes one first and three repeated store calls before
falling through to `$C30808`.

This is static arithmetic/control-flow evidence only; it does not establish
what the counter or resulting blits represent.
