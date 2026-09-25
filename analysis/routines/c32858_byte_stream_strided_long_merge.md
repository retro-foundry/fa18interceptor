# `$C32858-$C3287D`: byte-stream to strided-long merge loop

Classification: **runtime-backed dataflow/control flow**.

The run029 chipset-frame-993 trace executes this loop repeatedly before the
two `$C30D..` blitter submissions.  The exact loop body is reconstructed in
`source_amiga/observed/merge_byte_stream_into_strided_longs.asm`.

For each iteration it loads one byte from `A0`, rotates it into a longword,
right-shifts both it and an `$E0000000` mask by the caller's `D2`, and merges
the masked result into the existing longword at `A3`.  It advances the source
by one byte and the destination by `$28` before decrementing `D6`.

This establishes a CPU graphics-compositing path from a byte stream into a
40-byte-stride buffer.  The frame-993 caller's destination is an alternate
working buffer, so it is not yet evidence that this specific stream is KTS or
altitude glyph data.  Its source-byte and caller contracts are the next
appropriate targets.

The helper's formerly static-only zero-selector tail and shared epilogue are
now byte-exact in `source_amiga/observed/finish_c32806_mask_rows.asm` for
`$C32880-$C328A5`, completing the `$C32806-$C328A5` helper range.
