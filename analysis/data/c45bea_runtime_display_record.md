# `$C45BEA`: runtime display record, not an exportable static mesh

The Golden Gate pre-cull sheet formerly labelled `$C45BEA` has a recognisably
aircraft- or missile-like silhouette. Follow-up caller capture shows that
`$C45BEA` is its active transform/display context, while the actual face
records are `$C34C06-$C34C48`. The plot is now catalogued as the
[`$C34C` aircraft-detail family](c34c_aircraft_detail_face_family.md), not as
a `$C45BEA` model.

## Evidence

- `$C0D74A` explicitly loads `$C45BEA` as the alternate display-record base
  before entering the common display-record preparation routine; see
  [`select_alternate_display_matrix_base.asm`](../../source_amiga/observed/select_alternate_display_matrix_base.asm).
- The matrix-route reconstruction designates the same address
  `FIRST_MATRIX_CACHE`, and supplies it to the three-angle matrix composition
  routine; see
  [`update_control_record_matrix_route.asm`](../../source_amiga/observed/update_control_record_matrix_route.asm).
- Six sampled words change materially between replay checkpoints. At frame
  7500 they are `FF06 0029 0028 0020 00F9 FFD1`; at frame 12000 they are
  `0061 0002 00EC FFE9 00FE 0007`; and at frame 14200 they are
  `FF64 FFC4 00C1 FFE3 00F8 0037`. The samples are preserved in
  `build/run031_frame{7500,12000,14200}_c45bea_memory_sample*.json`.
- The six words remain unchanged for twelve no-input frames after the frame
  12000 checkpoint. Thus it is a persistent transformed/display record at
  that moment, rather than short-lived scratch space, but it varies with the
  scene/control state.
- The expanded `$C2469E` capture records five recurring pre-clip calls through
  `$C203C4` with `A5=$C45BEA`, but with `A2` equal to `$C34C06`, `$C34C18`,
  `$C34C2A`, `$C34C38`, and `$C34C48`. Therefore `A5` identifies a mutable
  transform/display context, not the polygon-face source.

## Conclusion

`$C45BEA` is renderer-consumable runtime data. It changes the pose/display of
the linked `$C34C` aircraft-detail faces but is not itself an exportable static
mesh. Keep it separate from static-model export candidates.
