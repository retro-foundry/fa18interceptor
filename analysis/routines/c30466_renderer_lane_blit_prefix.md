# `$C30466` renderer lane-blit prefix

Classification: **runtime-backed dataflow/control flow**. It is called by the
renderer lane stage and is present in `run001_c30466_renderer_child/` plus the
run001 and attract renderer-stage exports.

`source_amiga/observed/submit_renderer_lane_blit_prefix.asm` is byte-exact for
`$C30466-$C304B1` (76 bytes). It shifts `$C45956`, loads a lane-selected
pointer from `$C456B6`, derives blit inputs, waits for idle, then selects a
control-word continuation based on bit zero of `D4` and `D3`.

The continuation is reconstructed separately from `$C304B2`; this prefix does
not assign a graphics interpretation to the selected lane.
