# `$C2168A`: long cross-term projection-record preparation

Classification: **runtime-backed generic projection-record preparation**.

`source_amiga/observed/prepare_long_cross_term_projection_record.asm` is byte
exact for `$C2168A-$C217E9`.

## Contract

The helper begins with a selected `$C48390` table triple, then combines two
record-component deltas through the current component workspaces. Unlike the
nearby `$C2159E` variant, it retains a long signed cross-term while choosing
between two table-derived three-point layouts. Each layout performs an ANDed
depth rejection and submits accepted workspace data through `$C2469E`.

The signed bit-15 state from the second control-stream record offset is kept
in the high word of `D6` during the decision. `A1`, `A2`, and `A5` are saved
around projection; rejection returns `D0=0`.

## Runtime anchor

The frame-12,000 run031 Golden Gate control block invokes this entry with
selector `$8020` and payload at `$C35772` (trace index 1161). It is an active
scene-record handler, but is not yet assigned to an individual visible bridge
element.
