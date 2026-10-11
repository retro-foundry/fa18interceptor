# Tuple cache leaf at `$C248B2` (Hunk 19 +`$22A`)

Classification: **structural**. The complete observed path copies a three-word
tuple through caller-owned pointers and returns a status; tuple meaning is
unknown.

`source_amiga/observed/tuple_cache_stage.asm` is a byte-exact 208-byte
observed-entry slice for `$C248B2-$C24981`. It includes alternate static
interpolation and rounding paths; branches to `$C24982` leave the slice.

## Runtime packet

- Direct edge `$C2488C -> $C248B2 -> $C24890` within `$C247C0`.
- 13 instructions, complete at the return boundary.
- P-code: `pcode/raw/run001_c248b2_tuple_leaf/`, 13 starts / 86 operations,
  fully mapped to verified Hunk 19.

The observed path saves `D0-D2`, loads `(A3)` as three words, tests byte
`2(A4)`, copies the tuple to `26(A2)`, increments that byte, then stores the
tuple at `20(A2)`. It returns `D0 = 1`. Static alternate paths are unobserved.
