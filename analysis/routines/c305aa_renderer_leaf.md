# Renderer comparison leaf at `$C305AA`

Classification: **structural**. The run001 call `$C30324 -> $C305AA -> $C3032A`
completed in four instructions. The observed path compares `D1` with `D3`,
takes the equal branch to `$C305D4`, returns, and restores `A4` at the caller.

P-code: `pcode/raw/run001_c305aa_renderer_child/`, four observed starts /
15 operations. The unequal continuation at `$C305AE` is outside this bounded
path and has no source reconstruction yet.


`source_amiga/observed/prepare_unequal_pair_range.asm` is the byte-exact
44-byte entry slice `$C305AA-$C305D5`. It contains the equal return, the
static unequal-range preparation, and named external continuations at
`$C305D6` and `$C305F8`.
