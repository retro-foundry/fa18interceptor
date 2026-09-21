# Renderer child at `$C30466`

Classification: **structural**. The direct run001 edge `$C30026 -> $C30466 ->
$C3002A` completed in 28 instructions at frame 9, with no nested call target.

P-code: `pcode/raw/run001_c30466_renderer_child/`, 28 observed starts /
141 operations. The packet reads renderer globals, selects a custom-register block rooted at
`$DFF000`, and writes observed words/longwords at offsets `$40-$58` before
returning. It also tests bit 6 of the custom block at offset 2 and bit 0 of
working registers. The exact blitter/primitive role of this setup remains
unassigned.
