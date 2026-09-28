# `$C2DB18` control-record matrix route

Classification: **byte-exact static reconstruction with a capped normal-run
packet**. `$C2D99C` selects this route when `$C45785` is zero. The run003
frame-6,000 direct trace is capped at 3,000 instructions, so it does not
establish a complete dynamic return boundary.

The complete static `$C2DB18-$C2DCC1` body is recovered in
`source_amiga/observed/update_control_record_matrix_route.asm` (426 bytes).
It loads angle words from the active 512-byte control record, selects literal
or table tuple inputs based on structural state fields, builds/scales temporary
and cached matrices, and calls `$C2DEE0` with a record-relative context.

Names in the source describe data flow only. The state-byte, record-type, and
matrix-cache game-level roles remain unassigned pending differential traces.

## run075 prepared-page witness

The return-bounded `build/run075_frame373_c2d99c_matrix/` trace reaches the
dispatcher at global frame 380 with `$C45785=0`, so it executes this route.
It takes the default non-special, selector-zero, mode-at-most-one path:
`$C2DB9E` copies the active record's `+$66/+$68/+$6A` tuple to `$C45A88`,
then `$C2DCB2` calls `$C2E3DE` for `$C45BD8` with `(0,28600,0)`. `$C2DCBC`
immediately scales that matrix through `$C2E5AC` using live row scales
`(168,252,128)`, yielding `(167,0,-8 / 0,252,0 / 6,0,127)`.

This is the matrix consumed by the established run075 frame-384 projection
contract. It validates the existing default C route as the dynamic matrix
producer for that pass; it does not establish a normal C scheduler or permit
captured values in runtime code.

`default_scene_render_pass.{c,h}` composes this default route with the later
same-active-record `$C1C54E -> $C279D0` handoff. Its contract checks the
run075 matrix oracle and the renderer's ready-side initialization, while
keeping page selection, palette publication, and parent cadence caller-owned.
