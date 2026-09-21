# `$C3040C` renderer-stage prelude blit

Classification: **static-only dataflow**. It is called from the reconstructed
lane stage only under an unobserved non-negative/flag path.

`source_amiga/observed/submit_renderer_stage_prelude_blit.asm` is byte-exact
for `$C3040C-$C30465` (90 bytes). It reads two renderer pointers, offsets
them, waits for blitter idle, then submits one blit through the Custom register
block. The pointer/content meaning remains unassigned.
