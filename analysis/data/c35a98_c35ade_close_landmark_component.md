# `$C35A98` / `$C35ADE`: close Golden Gate primitive inputs

Classification: **trace-proven local immutable input pairs with
landmark-associated close-window output**.

In the sealed run041 close window, `$C1F4AC` enters independently at
`$C35A98` and `$C35ADE`. The transform loop's internal back-edge is
`$C1F528`, so its `$C1F524` entry alone does not delimit a packet. The
preceding packet count bytes are `$04` and `$08`; decoding exactly that many
big-endian signed-word triples gives the complete direct-input runs below:

| Direct range | Immutable triples | Bounded output |
| --- | --- | --- |
| `$C35A98-$C35AAF` | `(112,440,3168)`, `(0,440,3168)`, `(0,0,-3744)`, `(0,0,0)` | seven polygon submissions and two `$C355C2` line lists |
| `$C35ADE-$C35B0D` | `(112,440,-3168)`, `(0,440,-3168)`, `(0,0,3744)`, `(0,0,0)`, `(0,384,-1824)`, `(0,256,1184)`, `(0,1088,0)`, `(0,608,-1824)` | one `$C35590` line list |

Two focused no-input source-interval traces independently validate those
counts. `$C35A98` executes `$C1F4AC` once plus `$C1F528` three times, then
submits two `$C212B0` line lists with `A5=$C355C2`. `$C35ADE` executes
`$C1F4AC` once plus `$C1F528` seven times, then submits one line list with
`A5=$C35590`. This validates the whole direct-input run lengths without
mistaking `$C1F524`'s loop-entry check for the loop back-edge.

The first two triples in each run have the same Y and Z values, differing only
in X. Their opposite Z signs make those leading pairs a mirrored local input
arrangement. Several remaining triples have nonzero local Y, so the data does
not support the stronger claim that every local component is two-dimensional.
It still does **not** establish map axes, world coordinates, an elevation
model, or face connectivity.

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
`build/run041_red_6250_line_entries/blitter_line_entries.json`, plus
`build/run041_c35a98_source_interval/` and
`build/run041_c35ade_source_interval/`.
