# `$C2E3DE` three-angle matrix composer

Classification: **byte-exact static reconstruction**. The routine accepts
three native angle words in `D0.w`, `D2.w`, and `D4.w`, writes nine signed
matrix words through `A1`, and advances `A1` by 16 bytes.

It shifts all three angles by three, calls `$C2E5F6` to obtain two
sine/cosine pairs, then calls `$C2E6DA` for the third. The resulting products
are combined through the observed mixed `MULS`, arithmetic-shift, and `SWAP`
sequence. The local scaling sequence is intentionally preserved rather than
rewritten as an assumed algebraic matrix identity.

The complete `$C2E3DE-$C2E479` range (156 bytes) is in
`source_amiga/observed/compose_three_angle_matrix.asm` and assembles exactly
against the baseline runtime snapshot.
