# `$C2FDF4-$C2FEDA`: remaining active-plane submissions

This 234-byte exact continuation follows the first active-plane trigger at
`$C2FDF0`. It reads offsets 4, 8, and 12 from the same `A2` table selected by
the preceding `$C2FD8C` slice, adds `$28`, waits for blitter idle, writes the
result to `BLTCPT` and `BLTDPT`, and triggers `BLTSIZE` at `$C2FE3A`,
`$C2FE90`, and `$C2FEDA`.

The bounded cockpit trace records those trigger PCs and Custom-register
destinations. The live `$C4567E` table order established by the preceding
packet maps these entries to Copper planes 3, 2, and 1 respectively. The
second and third paths maintain observed busy-poll counters at `$C4591C` and
`$C45920`; the fourth maintains `$C45924`. `$C4589B` selects the third path's
alternate `BLTCON0` word.

This is static-plus-trace evidence for three active-plane submissions, not a
return-bounded contract for the enclosing packet or a claim about pixel
semantics. The following bytes at `$C2FEDE` remain outside this slice.
