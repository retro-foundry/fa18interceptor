# `$C306D4` segment-37 blitter sequence

Classification: **static-only dataflow**. Segment 37 begins at `$C306C0` with
a five-longword pointer table; this routine starts at `$C306D4`. No available
P-code export enters this entry.

`source_amiga/observed/submit_segment37_blitter_sequence.asm` is byte-exact
for `$C306D4-$C30751` (126 bytes). It walks the `$C456BA` pointer block and
the local `$C306C0` pointer table, invokes `$C53F44` for each pair, configures
the Custom blitter registers, and submits four values through the shared store
tail.

This captures control and pointer/register dataflow only; the table and blit
contents are not assigned a gameplay or graphics meaning.
