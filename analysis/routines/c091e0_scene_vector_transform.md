# `$C091E0`: scene-vector matrix transform

Classification: **byte-exact arithmetic primitive**.

`source_amiga/observed/initialize_c091e0_transform_entry.asm` establishes the
entry; static continuation `$C091F0-$C09248` reads a caller-owned nine-word
matrix at `+92`, multiplies three signed input words per output, wraps each
longword accumulation, arithmetic-shifts each sum by four, and adds caller
owned longword fields `+14/+18/+1C` with 68000 longword wrap.

The run075 `$C0924A` active path calls it with input `(11,0,$68)` before
committing its three returned longwords to the root record. That caller's
table selection and target ownership are separate from this primitive.

`port/scene_vector_transform.c` ports the primitive as
`fa18_transform_scene_vector`; its contract covers signed products, negative
arithmetic shift, translation, and null rejection. It does not conflate this
12.4-style longword transform with the existing distinct vertex transform.
