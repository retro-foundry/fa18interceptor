# Map polygon static-packet inventory

Classification: **bounded immutable input to map-page polygon transforms**.

Each row ends at a traced C2AFE2 display-stage entry and retains the preceding C2AF9C/C2AF9E fixed-point source-pair reads since the prior display-stage entry. Every such pair is in verified original segment 68. Only batches with an observed direct C2AEFC-to-C2AF00 entry expose a packet header; the first batch begins after the trace starts. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map.

- Original source segment: 68 `$C42CA8-$C444F7`
- Direct `$C2AF00` packet entries: 6
- Direct inline/alternate selections: 6 inline, 0 alternate
- Completed transform batches at `$C2AFE2`: 8
- Exact immutable coordinate pairs consumed: 41

## Transform batches

| Trace frame | Pair-address range | Direct packet header | Pairs | `$C2FF48` before next pair transform |
| ---: | --- | --- | ---: | ---: |
| 1 | `$C43DE4`--`$C43E0C` | not entered directly in this batch | 11 | 0 |
| 1 | `$C43E12`--`$C43E1E` | not entered directly in this batch | 4 | 0 |
| 1 | `$C43D20`--`$C43D2C` | `$C43D1A / $00C43D1E` | 4 | 1 |
| 1 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 1 |
| 1 | `$C439D2`--`$C439DE` | `$C439CC / $00C439D0` | 4 | 0 |
| 1 | `$C439D2`--`$C439DE` | `$C439CC / $00C439D0` | 4 | 0 |
| 1 | `$C439AE`--`$C439C6` | `$C439A8 / $00C439AC` | 7 | 0 |
| 1 | `$C4399A`--`$C439A2` | `$C43994 / $00C43998` | 3 | 0 |

The JSON companion retains every exact consumed signed pair and its static address.

## Direct packet stream selections

`$C2AF40` chooses the inline source at `header + 4` when `D7` is zero, otherwise the header longword.

| Frame | Header | Inline stream | Header pointer | `D7` | Selected stream | Route |
| ---: | --- | --- | --- | ---: | --- | --- |
| 1 | `$C43D1A` | `$C43D1E` | `$C43D1E` | 0 | `$C43D1E` | inline |
| 1 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 1 | `$C439CC` | `$C439D0` | `$C439D0` | 0 | `$C439D0` | inline |
| 1 | `$C439CC` | `$C439D0` | `$C439D0` | 0 | `$C439D0` | inline |
| 1 | `$C439A8` | `$C439AC` | `$C439AC` | 0 | `$C439AC` | inline |
| 1 | `$C43994` | `$C43998` | `$C43998` | 0 | `$C43998` | inline |

The same coordinate pairs are available as grouped `l` primitives in the [OBJ inspection export](../exports/run003_m_map_static_pair_batches.obj) for run003 only. It writes `(source_x, 0, source_y)` solely as a viewer convention, with no faces, path closure, global placement, or game-axis semantics inferred.
