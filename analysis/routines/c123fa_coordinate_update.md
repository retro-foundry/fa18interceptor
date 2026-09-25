# Coordinate-update helper at `$C123FA`

Classification: **dataflow**. `$C2D9BA` pushes six longword arguments and
calls this helper before copying its two output words from `$C45AC0/$C45AC2`
back to the matrix pipeline's `$C45A94/$C45A96` inputs.

The exact observed entry `$C123FA-$C12435` is in
`source_amiga/observed/initialize_c123fa_coordinate_update.asm`. It allocates
a 28-byte local frame, clears a local sign-byte, and conditionally negates the
longwords at argument offsets `+$10` and `+$14`, recording their signs in
bits 0 and 1. It then tests the component at `+$18`; the negative branch is
not covered by this capture and is intentionally outside the slice.

Authority: `pcode/raw/no_key_c2d99c/observed.asm.txt` and
`source_amiga/observed/update_matrix_pipeline.asm`. These facts prove signed
component normalization at the entry and the two-word caller handoff. They do
not yet prove axes, units, or the helper's final coordinate formula.

Two executed scaling fragments are also exact: `$C124EE-$C12517` in
`prepare_c123fa_scaled_components.asm` applies the local shift count to the
arguments at `+$10` and `+$18`; `$C1256A-$C125C3` in
`calculate_c123fa_scaled_components.asm` publishes a dividend/divisor to
`$C45ACC/$C45AD0`, calls `$C25980`, uses the quotient at `$C45AD2` to index
`$C3DB00`, and preserves a sign-bit subset for later branches. This proves a
fixed-point divide-and-table intermediate, not a final angle or coordinate
meaning.

The observed return block `$C12934-$C1294F` is exact source in
`publish_c123fa_coordinate_outputs.asm`: it publishes the low words of its
two local longword results to `$C45AC0/$C45AC2`, restores `D2`, and returns.
Together with the direct caller copy in `$C2D9BA`, this makes the bounded
two-word handoff exact at both callee and caller boundaries.
