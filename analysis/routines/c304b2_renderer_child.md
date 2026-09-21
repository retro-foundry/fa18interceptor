# Renderer child at `$C304B2`

Classification: **structural**. The direct run001 edge `$C3002A -> $C304B2 ->
$C3002E` completes in 14 instructions at frame 9, with no nested calls.

P-code: `pcode/raw/run001_c304b2_renderer_child/`, 14 observed starts /
78 operations. Its register and custom-register effects remain separate from
the sibling `$C30466` setup path until their shared caller contract is traced.


`source_amiga/observed/setup_blitter_operation.asm` is the byte-exact 72-byte
function `$C304B2-$C304F9`. It waits on custom-register bit 6, programs
`BLTCON0/1`, `BLTCPT`, `BLTBPT`, `BLTAPT`, and `BLTSIZE`, then returns.
