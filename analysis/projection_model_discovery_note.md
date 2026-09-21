# Projection-first model discovery note

Perspective projection is a high-leverage RE target: once its input-record
contract is proven, the producer tables can be searched for coherent 3D model
geometry and their rendered art can be identified from deterministic frames.

Current evidence bounds projected-segment preparation at `$C2EE4A`, the
matrix-product projection path at `$C2ECC6`, and now the runtime-connected
`$C2F03A` perspective tail. The latter proves the exact 320×180 projection
formula from `$C45AC6` through `$C2FA7E` line submission. Model/table ownership
remains unproven; the paired-triple producer is now `$C212B0`, which resolves
offset pairs through `$C48390`. Its frame-602 caller supplies the stable
ten-segment candidate list at `$C3985A`, documented in
`analysis/data/c3985a_projected_edge_list.md`. The next step is to trace the
caller that chooses this list and distinguish static model tables from
per-frame transformed instance data at `$C48390`.

## Run031 polygon path

The Golden Gate-frame control records also reach a second exact perspective
tail at `$C24CFE`: clipped triples at `$C4B990` become screen pairs at
`$C4B390`, then `$C2FF48` receives the completed list. This path uses the
same 320-by-180 viewport scales as the segment tail (`160` for X and `90` for
Y), clamps the screen coordinates, and stores reflected coordinates
`(319 - x, 179 - y)`.

The two tails therefore share a screen-space convention but accept different
intermediate formats. The next required evidence is output-to-pixel
correlation for specific `$C48390` records, not further speculative model
naming.
