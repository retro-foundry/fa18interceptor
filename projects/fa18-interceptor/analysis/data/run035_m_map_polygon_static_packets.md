# Map polygon static-packet inventory

Classification: **bounded immutable input to map-page polygon transforms**.

Each row ends at a traced C2AFE2 display-stage entry and retains the preceding C2AF9C/C2AF9E fixed-point source-pair reads since the prior display-stage entry. Every such pair is in verified original segment 68. Only batches with an observed direct C2AEFC-to-C2AF00 entry expose a packet header; the first batch begins after the trace starts. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map.

- Original source segment: 68 `$C42CA8-$C444F7`
- Direct `$C2AF00` packet entries: 18
- Direct inline/alternate selections: 8 inline, 10 alternate
- Completed transform batches at `$C2AFE2`: 21
- Exact immutable coordinate pairs consumed: 171

## Transform batches

| Trace frame | Pair-address range | Direct packet header | Pairs | `$C2FF48` before next pair transform |
| ---: | --- | --- | ---: | ---: |
| 8856 | `$C43CFC`--`$C43D14` | `$C43CBE / $00C43CFA` | 7 | 1 |
| 8856 | `$C43C72`--`$C43C8A` | `$C43BAC / $00C43C70` | 7 | 1 |
| 8856 | `$C43C90`--`$C43CB8` | not entered directly in this batch | 11 | 0 |
| 8860 | `$C42D2E`--`$C42D62` | `$C42D28 / $00C42D2C` | 14 | 1 |
| 8860 | `$C42E20`--`$C42E38` | `$C42DFC / $00C42E00` | 7 | 1 |
| 8860 | `$C42DCA`--`$C42DF6` | `$C42DC4 / $00C42DC8` | 12 | 0 |
| 8860 | `$C43FB0`--`$C43FB8` | `$C43F5E / $00C43FAE` | 3 | 1 |
| 8860 | `$C43FBE`--`$C43FCA` | not entered directly in this batch | 4 | 1 |
| 8860 | `$C43F3C`--`$C43F58` | `$C43ECA / $00C43F3A` | 8 | 0 |
| 8861 | `$C43CFC`--`$C43D14` | `$C43CBE / $00C43CFA` | 7 | 1 |
| 8861 | `$C43C72`--`$C43C8A` | `$C43BAC / $00C43C70` | 7 | 1 |
| 8861 | `$C43C90`--`$C43CB8` | not entered directly in this batch | 11 | 0 |
| 8864 | `$C42D2E`--`$C42D62` | `$C42D28 / $00C42D2C` | 14 | 1 |
| 8865 | `$C42E20`--`$C42E38` | `$C42DFC / $00C42E00` | 7 | 1 |
| 8865 | `$C42DCA`--`$C42DF6` | `$C42DC4 / $00C42DC8` | 12 | 0 |
| 8865 | `$C43FB0`--`$C43FB8` | `$C43F5E / $00C43FAE` | 3 | 1 |
| 8865 | `$C43FBE`--`$C43FCA` | not entered directly in this batch | 4 | 1 |
| 8865 | `$C43F3C`--`$C43F58` | `$C43ECA / $00C43F3A` | 8 | 0 |
| 8865 | `$C43CFC`--`$C43D14` | `$C43CBE / $00C43CFA` | 7 | 1 |
| 8866 | `$C43C72`--`$C43C8A` | `$C43BAC / $00C43C70` | 7 | 1 |
| 8866 | `$C43C90`--`$C43CB8` | not entered directly in this batch | 11 | 0 |

The JSON companion retains every exact consumed signed pair and its static address.

## Direct packet stream selections

`$C2AF40` chooses the inline source at `header + 4` when `D7` is zero, otherwise the header longword.

| Frame | Header | Inline stream | Header pointer | `D7` | Selected stream | Route |
| ---: | --- | --- | --- | ---: | --- | --- |
| 8856 | `$C43CBE` | `$C43CC2` | `$C43CFA` | 14004 | `$C43CFA` | alternate |
| 8856 | `$C43BAC` | `$C43BB0` | `$C43C70` | 1 | `$C43C70` | alternate |
| 8859 | `$C42D28` | `$C42D2C` | `$C42D2C` | 0 | `$C42D2C` | inline |
| 8860 | `$C42DFC` | `$C42E00` | `$C42E00` | 0 | `$C42E00` | inline |
| 8860 | `$C42E1A` | `$C42E1E` | `$C42E1E` | 0 | `$C42E1E` | inline |
| 8860 | `$C42DC4` | `$C42DC8` | `$C42DC8` | 0 | `$C42DC8` | inline |
| 8860 | `$C43F5E` | `$C43F62` | `$C43FAE` | 55784 | `$C43FAE` | alternate |
| 8860 | `$C43ECA` | `$C43ECE` | `$C43F3A` | 1 | `$C43F3A` | alternate |
| 8861 | `$C43CBE` | `$C43CC2` | `$C43CFA` | 55784 | `$C43CFA` | alternate |
| 8861 | `$C43BAC` | `$C43BB0` | `$C43C70` | 1 | `$C43C70` | alternate |
| 8864 | `$C42D28` | `$C42D2C` | `$C42D2C` | 0 | `$C42D2C` | inline |
| 8865 | `$C42DFC` | `$C42E00` | `$C42E00` | 0 | `$C42E00` | inline |
| 8865 | `$C42E1A` | `$C42E1E` | `$C42E1E` | 0 | `$C42E1E` | inline |
| 8865 | `$C42DC4` | `$C42DC8` | `$C42DC8` | 0 | `$C42DC8` | inline |
| 8865 | `$C43F5E` | `$C43F62` | `$C43FAE` | 32028 | `$C43FAE` | alternate |
| 8865 | `$C43ECA` | `$C43ECE` | `$C43F3A` | 1 | `$C43F3A` | alternate |
| 8865 | `$C43CBE` | `$C43CC2` | `$C43CFA` | 32028 | `$C43CFA` | alternate |
| 8866 | `$C43BAC` | `$C43BB0` | `$C43C70` | 1 | `$C43C70` | alternate |

The same coordinate pairs are available as grouped `l` primitives in the [OBJ inspection export](../exports/run003_m_map_static_pair_batches.obj) for run003 only. It writes `(source_x, 0, source_y)` solely as a viewer convention, with no faces, path closure, global placement, or game-axis semantics inferred.
