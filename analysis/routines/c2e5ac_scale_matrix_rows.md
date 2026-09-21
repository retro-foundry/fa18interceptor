# `scale_matrix_rows` at `$C2E5AC` (Hunk 32 +`$1A4`)

Classification: **behavioural**. This routine applies a three-word fixed-point
scale vector to the rows of a 3×3 word matrix.

It loads three signed scale words from `$C45A3E`, then processes three
consecutive groups of three words through `A1`. Each row uses one scale word:
every signed product is arithmetically shifted right by eight and written back.
`A1` advances by 12 bytes, ending at the third row.

The complete 74-byte source is
[`source_amiga/observed/scale_matrix_rows.asm`](../../source_amiga/observed/scale_matrix_rows.asm)
and matches the runtime bytes exactly.

The no-input packet calls it from `$C2DACC` and returns to `$C2DAD0` after 28
instructions. At this invocation it processes matrix rows at `$C45BD8`,
`$C45BDE`, and `$C45BE4`. Its P-code packet,
`pcode/raw/no_key_c2e5ac/`, has 28 instructions / 346 operations, all mapped
to verified Hunk 32. Static Hunk-32 calls also exist at `$C2DC80` and
`$C2DCBC`; matrix ownership and scale-vector semantics remain unknown.
