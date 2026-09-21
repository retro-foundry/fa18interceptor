# `$C32B40` first postflight transform-lane submission

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this complete `$C32B40-$C32B71` first-lane path.

`source_amiga/observed/submit_first_postflight_transform_lane.asm` is
byte-exact. It doubles the prepared row byte for a word-offset lookup at
`$C3D790`, combines the selected lane pointer with the transformed offset,
rejects an odd result, chooses `$0B0A` or `$0BFA` from bit 3 of `$C45955`, ORs
the table's second word into the mask, and calls `$C330FE`.

This proves the transform path feeds the existing strided-long mask-update
packet. Table semantics and the visual meaning of individual lanes remain
unproven.
