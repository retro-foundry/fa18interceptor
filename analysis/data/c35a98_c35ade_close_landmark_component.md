# `$C35A98` / `$C35ADE`: close Golden Gate primitive inputs

Classification: **trace-proven local immutable input pairs with
landmark-associated close-window output**.

In the sealed run041 close window, `$C1F4AC` enters independently at
`$C35A98` and `$C35ADE`. The first `$C1F524` loop observation advances `A1`
by six bytes, proving the leading pairs below. The transform loop's internal
back-edge is `$C1F528`, so the one observed `$C1F524` entry must **not** be
mistaken for the complete count. The preceding packet count bytes are `$04`
and `$08`, giving four and eight directly transformed triples respectively:

| Leading range | Leading immutable triples | Complete direct-input run | Bounded output |
| --- | --- | --- |
| `$C35A98-$C35AA3` | `(112,440,3168)`, `(0,440,3168)` | `$C35A98-$C35AAF` (four triples) | seven polygon submissions and two `$C355C2` line lists |
| `$C35ADE-$C35AE9` | `(112,440,-3168)`, `(0,440,-3168)` | `$C35ADE-$C35B0D` (eight triples) | one `$C35590` line list |

The two leading triples in each pair have the same Y and Z values, differing
only in X. Their opposite Z signs make the leading pairs a mirrored local input
arrangement; they do **not** establish map axes, world coordinates, or an
elevation model. The remaining directly transformed triples are deliberately
not exported here: the trace proves their count, but not their local topology.

`$C35A98` is the close-window member with direct raster evidence. Its bounded
renderer interval produces polygon workspace `$C4BFA6`; the replay-preserved
line emitter records an edge `(123,90)->(177,91)` from that workspace entirely
inside the measured red Golden Gate bitmap rectangle `x=54..224, y=90..104`.
This establishes local component-to-landmark ownership for that emitted edge,
not ownership of every polygon or a complete bridge model.

The portable direct-input export is
[`c35a98_c35ade_close_landmark_component_payload.json`](c35a98_c35ade_close_landmark_component_payload.json).
Its strict boundary keeps static inputs separate from the following control
words and all mutable renderer workspaces.

Authority: sealed `captures/run041`; ignored
`build/run041_frame06250_12f_trace_retry/trace.jsonl`,
`build/run041_large_view_matrix_instances/instance_geometry.json`, and
`build/run041_red_6250_line_entries/blitter_line_entries.json`.
