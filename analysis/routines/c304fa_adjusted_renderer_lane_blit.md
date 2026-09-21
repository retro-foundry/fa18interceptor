# `$C304FA` adjusted renderer lane blit

Classification: **static-only dataflow**. This `$C304FA-$C305A9` sibling is
called by the static path at `$C301E6`; no P-code export enters it directly.

`source_amiga/observed/submit_adjusted_renderer_lane_blit.asm` is byte-exact
for the complete 176-byte routine. It selects a renderer pointer through
`$C456B6`, derives a pointer from `$C45982/$C458D8`, adjusts its blit control
from `$C4597C/$C45986`, waits for blitter idle, and submits the resulting
register values through the Custom block.

This documents only the arithmetic and hardware-register dataflow, not the
game or graphics meaning of the resulting blit.
