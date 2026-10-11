# `build_two_angle_matrix` at `$C2E38E` (Hunk 32 +`$F86`)

Classification: **behavioural**. This helper builds a nine-word matrix from two
native angle words. It is distinct from the three-angle `$4000`-unity matrix
created by `build_rotation_matrix`.

It shifts `D0.w` and `D2.w` by three, calls `lookup_two_sine_cosine`, then
writes nine words through `A1`. Direct trig terms are shifted by six; product
terms are formed with `MULS`, `SWAP`, and arithmetic shift by four. The exact
80-byte reconstruction is
[`source_amiga/observed/build_two_angle_matrix.asm`](../../source_amiga/observed/build_two_angle_matrix.asm).

The no-input update packet calls it from `$C2DAC2`, returns to `$C2DAC6` after
59 instructions, and writes its matrix at `$C45BD8`. Its P-code packet,
`pcode/raw/no_key_c2e38e/`, contains 59 instructions / 568 operations, all
mapped to verified Hunk 32. `$C2E5AC` immediately scales this matrix in the
same observed path. The two angles’ game-level meanings are unknown.
