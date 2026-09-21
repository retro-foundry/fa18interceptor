# `$C2E514` alternate three-angle matrix composer

Classification: **byte-exact static reconstruction**. The bounded matrix
pipeline calls this routine at `$C2D996` after preparing three angles. It
shifts the native inputs, obtains two sine/cosine pairs through `$C2E5F6` and
the third through `$C2E6DA`, then writes nine signed words through `A1`.

Its product order and signs differ from the neighbouring `$C2E3DE` composer,
so it is kept as a distinct routine rather than renamed as a generic rotation
formula. The complete `$C2E514-$C2E5AB` range (152 bytes) is
`source_amiga/observed/compose_alternate_three_angle_matrix.asm`.
