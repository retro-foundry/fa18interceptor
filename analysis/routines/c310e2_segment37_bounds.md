# `$C310E2` segment-37 bounds helper

Classification: **partially runtime-backed dataflow**. The `attract_1800`
P-code export calls this helper from `$C30EBC` and records 32 body bytes;
remaining branches are static byte evidence.

`source_amiga/observed/adjust_segment37_table_renderer_bounds.asm` reproduces
`$C310E2-$C3111F` (62 bytes). It reads `$C45986`, combines it with `d7`,
conditionally adjusts `d1`, clears `d7` on one path, and returns either a
derived `d5`, zero, or `-1`. The caller branches on the signed condition after
this return.

The screen-space interpretation of the bounds and offsets is not proven;
the routine name states only its demonstrated arithmetic contract.
