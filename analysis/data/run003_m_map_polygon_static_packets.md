# Map polygon static-packet inventory

Classification: **bounded immutable input to map-page polygon transforms**.

Each row ends at a traced C2AFE2 display-stage entry and retains the preceding C2AF9C/C2AF9E fixed-point source-pair reads since the prior display-stage entry. Every such pair is in verified original segment 68. Only batches with an observed direct C2AEFC-to-C2AF00 entry expose a packet header; the first batch begins after the trace starts. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map.

- Original source segment: 68 `$C42CA8-$C444F7`
- Direct `$C2AF00` packet entries: 26
- Direct inline/alternate selections: 26 inline, 0 alternate
- Completed transform batches at `$C2AFE2`: 55
- Exact immutable coordinate pairs consumed: 353

## Transform batches

| Trace frame | Pair-address range | Direct packet header | Pairs | `$C2FF48` before next pair transform |
| ---: | --- | --- | ---: | ---: |
| 1 | `$C42D4A`--`$C42D62` | not entered directly in this batch | 7 | 1 |
| 1 | `$C42D6A`--`$C42D76` | not entered directly in this batch | 4 | 1 |
| 1 | `$C42D7E`--`$C42D8A` | not entered directly in this batch | 4 | 1 |
| 1 | `$C42D92`--`$C42DAA` | not entered directly in this batch | 7 | 1 |
| 1 | `$C42DB2`--`$C42DBE` | not entered directly in this batch | 4 | 1 |
| 2 | `$C42E04`--`$C42E14` | `$C42DFC / $00C42E00` | 5 | 0 |
| 2 | `$C42E20`--`$C42E38` | `$C42E1A / $00C42E1E` | 7 | 1 |
| 2 | `$C42DCA`--`$C42DF6` | `$C42DC4 / $00C42DC8` | 12 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 2 | `$C43ED0`--`$C43EF0` | `$C43ECA / $00C43F3A` | 9 | 1 |
| 2 | `$C43EF6`--`$C43F16` | not entered directly in this batch | 9 | 1 |
| 3 | `$C43F1C`--`$C43F34` | not entered directly in this batch | 7 | 1 |
| 3 | `$C43E2A`--`$C43E3A` | `$C43E24 / $00C43E9A` | 5 | 1 |
| 3 | `$C43E40`--`$C43E5C` | not entered directly in this batch | 8 | 0 |
| 3 | `$C43E62`--`$C43E7E` | not entered directly in this batch | 8 | 1 |
| 3 | `$C43E84`--`$C43E94` | not entered directly in this batch | 5 | 1 |
| 3 | `$C43D8A`--`$C43D92` | `$C43D84 / $00C43D88` | 3 | 0 |
| 3 | `$C43D98`--`$C43DB0` | not entered directly in this batch | 7 | 1 |
| 3 | `$C43DB6`--`$C43DDE` | not entered directly in this batch | 11 | 1 |
| 4 | `$C43DE4`--`$C43E0C` | not entered directly in this batch | 11 | 1 |
| 4 | `$C43E12`--`$C43E1E` | not entered directly in this batch | 4 | 0 |
| 4 | `$C43BB2`--`$C43BCA` | `$C43BAC / $00C43C70` | 7 | 1 |
| 4 | `$C43BD0`--`$C43BF4` | not entered directly in this batch | 10 | 1 |
| 4 | `$C43BFA`--`$C43C22` | not entered directly in this batch | 11 | 1 |
| 5 | `$C43C28`--`$C43C4C` | not entered directly in this batch | 10 | 1 |
| 5 | `$C43C52`--`$C43C6A` | not entered directly in this batch | 7 | 1 |
| 5 | `$C43B3A`--`$C43B56` | `$C43B34 / $00C43B94` | 8 | 1 |
| 5 | `$C43B5C`--`$C43B78` | not entered directly in this batch | 8 | 1 |
| 5 | `$C43B7E`--`$C43B8E` | not entered directly in this batch | 5 | 1 |
| 6 | `$C43A7E`--`$C43A9E` | `$C43A78 / $00C43AF0` | 9 | 1 |
| 6 | `$C43AA4`--`$C43AC4` | not entered directly in this batch | 9 | 1 |
| 6 | `$C43ACA`--`$C43AEA` | not entered directly in this batch | 9 | 1 |
| 6 | `$C439EE`--`$C43A06` | `$C439E8 / $00C43A44` | 7 | 1 |
| 6 | `$C43A0C`--`$C43A1C` | not entered directly in this batch | 5 | 1 |
| 7 | `$C43A22`--`$C43A3E` | not entered directly in this batch | 8 | 1 |
| 7 | `$C440A4`--`$C440B4` | `$C4409E / $00C440A2` | 5 | 0 |
| 7 | `$C44082`--`$C4408A` | `$C4407C / $00C44080` | 3 | 0 |
| 7 | `$C44090`--`$C44098` | not entered directly in this batch | 3 | 0 |
| 7 | `$C4406C`--`$C44074` | `$C44066 / $00C4406A` | 3 | 0 |
| 7 | `$C43F64`--`$C43F78` | `$C43F5E / $00C43FAE` | 6 | 1 |
| 7 | `$C43F7E`--`$C43F86` | not entered directly in this batch | 3 | 1 |
| 7 | `$C43F8C`--`$C43FA8` | not entered directly in this batch | 8 | 0 |
| 7 | `$C43D3A`--`$C43D4A` | `$C43D34 / $00C43D38` | 5 | 0 |
| 7 | `$C43D50`--`$C43D68` | not entered directly in this batch | 7 | 1 |
| 7 | `$C43D6E`--`$C43D7E` | not entered directly in this batch | 5 | 1 |
| 8 | `$C43CC4`--`$C43CF4` | `$C43CBE / $00C43CFA` | 13 | 1 |
| 8 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 1 |
| 8 | `$C439D2`--`$C439DE` | `$C439CC / $00C439D0` | 4 | 1 |
| 8 | `$C439AE`--`$C439C6` | `$C439A8 / $00C439AC` | 7 | 1 |
| 8 | `$C4399A`--`$C439A2` | `$C43994 / $00C43998` | 3 | 5 |

The JSON companion retains every exact consumed signed pair and its static address.

## Direct packet stream selections

`$C2AF40` chooses the inline source at `header + 4` when `D7` is zero, otherwise the header longword.

| Frame | Header | `A4` live-base pointer | `4(A4)` live term | Inline stream | Header pointer | `D7` | Selected stream | Route |
| ---: | --- | --- | ---: | --- | --- | ---: | --- | --- |
| 2 | `$C42DFC` | `$C45C3E` | 50331648 | `$C42E00` | `$C42E00` | 0 | `$C42E00` | inline |
| 2 | `$C42E1A` | `$C45C3E` | 50331648 | `$C42E1E` | `$C42E1E` | 0 | `$C42E1E` | inline |
| 2 | `$C42DC4` | `$C45C3E` | 50331648 | `$C42DC8` | `$C42DC8` | 0 | `$C42DC8` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C42E52` | `$C45C3E` | 50331648 | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 2 | `$C43ECA` | `$C45C3E` | 50331648 | `$C43ECE` | `$C43F3A` | 0 | `$C43ECE` | inline |
| 3 | `$C43E24` | `$C45C3E` | 50331648 | `$C43E28` | `$C43E9A` | 0 | `$C43E28` | inline |
| 3 | `$C43D84` | `$C45C3E` | 50331648 | `$C43D88` | `$C43D88` | 0 | `$C43D88` | inline |
| 4 | `$C43BAC` | `$C45C3E` | 50331648 | `$C43BB0` | `$C43C70` | 0 | `$C43BB0` | inline |
| 5 | `$C43B34` | `$C45C3E` | 50331648 | `$C43B38` | `$C43B94` | 0 | `$C43B38` | inline |
| 6 | `$C43A78` | `$C45C3E` | 50331648 | `$C43A7C` | `$C43AF0` | 0 | `$C43A7C` | inline |
| 6 | `$C439E8` | `$C45C3E` | 50331648 | `$C439EC` | `$C43A44` | 0 | `$C439EC` | inline |
| 7 | `$C4409E` | `$C45C3E` | 50331648 | `$C440A2` | `$C440A2` | 0 | `$C440A2` | inline |
| 7 | `$C4407C` | `$C45C3E` | 50331648 | `$C44080` | `$C44080` | 0 | `$C44080` | inline |
| 7 | `$C44066` | `$C45C3E` | 50331648 | `$C4406A` | `$C4406A` | 0 | `$C4406A` | inline |
| 7 | `$C43F5E` | `$C45C3E` | 50331648 | `$C43F62` | `$C43FAE` | 0 | `$C43F62` | inline |
| 7 | `$C43D34` | `$C45C3E` | 50331648 | `$C43D38` | `$C43D38` | 0 | `$C43D38` | inline |
| 8 | `$C43CBE` | `$C45C3E` | 50331648 | `$C43CC2` | `$C43CFA` | 0 | `$C43CC2` | inline |
| 8 | `$C43B1A` | `$C45C3E` | 50331648 | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 8 | `$C439CC` | `$C45C3E` | 50331648 | `$C439D0` | `$C439D0` | 0 | `$C439D0` | inline |
| 8 | `$C439A8` | `$C45C3E` | 50331648 | `$C439AC` | `$C439AC` | 0 | `$C439AC` | inline |
| 8 | `$C43994` | `$C45C3E` | 50331648 | `$C43998` | `$C43998` | 0 | `$C43998` | inline |

`A4` is captured at the direct `$C2AF00` entry, before the routine reads `4(A4)` as its per-packet projection-origin input.  The table records that live dataflow without treating the value as a packet placement record or a global map coordinate.

The same coordinate pairs are available as grouped `l` primitives in the [OBJ inspection export](../exports/run003_m_map_static_pair_batches.obj) for run003 only. It writes `(source_x, 0, source_y)` solely as a viewer convention, with no faces, path closure, global placement, or game-axis semantics inferred.
