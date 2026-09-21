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
