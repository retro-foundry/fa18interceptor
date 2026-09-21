# `$C32B40` first postflight transform-lane submission

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation executes the normal `$C32B40-$C32B71` submission
path; the odd-result rejection branch at `$C32B58` is unobserved.

`source_amiga/observed/submit_first_postflight_transform_lane.asm` is
byte-exact. It doubles the prepared row byte for a word-offset lookup at
`$C3D790`, combines the selected lane pointer with the transformed offset,
rejects an odd result, chooses `$0B0A` or `$0BFA` from bit 3 of `$C45955`, ORs
the table's second word into the mask, and calls `$C330FE`.

`$C330FE` consumes `D4` as a byte-stream pointer, so the `$C3D790` lookup
proves this path selects a glyph stream before feeding the existing
strided-long font compositor. The coordinate-table semantics and the visual
field remain unproven.
