# Map polygon static-packet inventory

Classification: **bounded immutable input to map-page polygon transforms**.

Each row ends at a traced C2AFE2 display-stage entry and retains the preceding C2AF9C/C2AF9E fixed-point source-pair reads since the prior display-stage entry. Every such pair is in verified original segment 68. Only batches with an observed direct C2AEFC-to-C2AF00 entry expose a packet header; the first batch begins after the trace starts. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map.

- Original source segment: 68 `$C42CA8-$C444F7`
- Direct `$C2AF00` packet entries: 57
- Direct inline/alternate selections: 57 inline, 0 alternate
- Completed transform batches at `$C2AFE2`: 110
- Exact immutable coordinate pairs consumed: 749

## Transform batches

| Trace frame | Pair-address range | Direct packet header | Pairs | `$C2FF48` before next pair transform |
| ---: | --- | --- | ---: | ---: |
| 25 | `$C43B42`--`$C43B56` | not entered directly in this batch | 6 | 1 |
| 25 | `$C43B5C`--`$C43B78` | not entered directly in this batch | 8 | 1 |
| 25 | `$C43B7E`--`$C43B8E` | not entered directly in this batch | 5 | 0 |
| 25 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 25 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 25 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 25 | `$C43A7E`--`$C43A9E` | `$C43A78 / $00C43AF0` | 9 | 0 |
| 25 | `$C43AA4`--`$C43AC4` | not entered directly in this batch | 9 | 0 |
| 25 | `$C43ACA`--`$C43AEA` | not entered directly in this batch | 9 | 0 |
| 25 | `$C439EE`--`$C43A06` | `$C439E8 / $00C43A44` | 7 | 0 |
| 25 | `$C43A0C`--`$C43A1C` | not entered directly in this batch | 5 | 0 |
| 25 | `$C43A22`--`$C43A3E` | not entered directly in this batch | 8 | 0 |
| 29 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 29 | `$C42E44`--`$C42E4C` | `$C42E3E / $00C42E42` | 3 | 0 |
| 29 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 30 | `$C42D2E`--`$C42D62` | `$C42D28 / $00C42D2C` | 14 | 1 |
| 30 | `$C42D6A`--`$C42D76` | not entered directly in this batch | 4 | 1 |
| 30 | `$C42D7E`--`$C42D8A` | not entered directly in this batch | 4 | 1 |
| 31 | `$C42D92`--`$C42DAA` | not entered directly in this batch | 7 | 1 |
| 31 | `$C42DB2`--`$C42DBE` | not entered directly in this batch | 4 | 1 |
| 31 | `$C42E04`--`$C42E14` | `$C42DFC / $00C42E00` | 5 | 0 |
| 31 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 31 | `$C42E20`--`$C42E38` | `$C42E1A / $00C42E1E` | 7 | 0 |
| 31 | `$C42DCA`--`$C42DF6` | `$C42DC4 / $00C42DC8` | 12 | 0 |
| 31 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 31 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 31 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 31 | `$C440C2`--`$C440DA` | `$C440BC / $00C440C0` | 7 | 1 |
| 31 | `$C440E0`--`$C44108` | not entered directly in this batch | 11 | 1 |
| 32 | `$C4410E`--`$C4412A` | not entered directly in this batch | 8 | 1 |
| 32 | `$C44130`--`$C44144` | not entered directly in this batch | 6 | 1 |
| 32 | `$C4414A`--`$C4415A` | not entered directly in this batch | 5 | 1 |
| 32 | `$C440A4`--`$C440B4` | `$C4409E / $00C440A2` | 5 | 1 |
| 32 | `$C43FD6`--`$C44006` | `$C43FD0 / $00C43FD4` | 13 | 1 |
| 33 | `$C4400C`--`$C44034` | not entered directly in this batch | 11 | 1 |
| 33 | `$C4403A`--`$C44046` | not entered directly in this batch | 4 | 1 |
| 33 | `$C43F64`--`$C43F78` | `$C43F5E / $00C43FAE` | 6 | 1 |
| 33 | `$C43F7E`--`$C43F86` | not entered directly in this batch | 3 | 1 |
| 33 | `$C43F8C`--`$C43FA8` | not entered directly in this batch | 8 | 1 |
| 33 | `$C43ED0`--`$C43EF0` | `$C43ECA / $00C43F3A` | 9 | 1 |
| 34 | `$C43EF6`--`$C43F16` | not entered directly in this batch | 9 | 1 |
| 34 | `$C43F1C`--`$C43F34` | not entered directly in this batch | 7 | 1 |
| 34 | `$C43D20`--`$C43D2C` | `$C43D1A / $00C43D1E` | 4 | 1 |
| 34 | `$C43CC4`--`$C43CF4` | `$C43CBE / $00C43CFA` | 13 | 1 |
| 35 | `$C43BB2`--`$C43BCA` | `$C43BAC / $00C43C70` | 7 | 1 |
| 35 | `$C43BD0`--`$C43BF4` | not entered directly in this batch | 10 | 1 |
| 35 | `$C43BFA`--`$C43C22` | not entered directly in this batch | 11 | 1 |
| 35 | `$C43C28`--`$C43C4C` | not entered directly in this batch | 10 | 1 |
| 36 | `$C43C52`--`$C43C6A` | not entered directly in this batch | 7 | 1 |
| 36 | `$C44250`--`$C44280` | `$C4424A / $00C4424E` | 13 | 1 |
| 36 | `$C44212`--`$C4422E` | `$C4420C / $00C44210` | 8 | 0 |
| 36 | `$C44234`--`$C44244` | not entered directly in this batch | 5 | 1 |
| 36 | `$C44180`--`$C441A0` | `$C4417A / $00C4417E` | 9 | 0 |
| 36 | `$C441A6`--`$C441D2` | not entered directly in this batch | 12 | 0 |
| 36 | `$C441D8`--`$C44204` | not entered directly in this batch | 12 | 0 |
| 36 | `$C44166`--`$C44172` | `$C44160 / $00C44164` | 4 | 1 |
| 37 | `$C44082`--`$C4408A` | `$C4407C / $00C44080` | 3 | 1 |
| 37 | `$C44090`--`$C44098` | not entered directly in this batch | 3 | 1 |
| 37 | `$C44052`--`$C4405E` | `$C4404C / $00C44050` | 4 | 1 |
| 37 | `$C43E2A`--`$C43E3A` | `$C43E24 / $00C43E9A` | 5 | 1 |
| 37 | `$C43E40`--`$C43E5C` | not entered directly in this batch | 8 | 1 |
| 37 | `$C43E62`--`$C43E7E` | not entered directly in this batch | 8 | 1 |
| 37 | `$C43E84`--`$C43E94` | not entered directly in this batch | 5 | 1 |
| 38 | `$C43D20`--`$C43D2C` | `$C43D1A / $00C43D1E` | 4 | 1 |
| 38 | `$C43B3A`--`$C43B56` | `$C43B34 / $00C43B94` | 8 | 1 |
| 38 | `$C43B5C`--`$C43B78` | not entered directly in this batch | 8 | 1 |
| 38 | `$C43B7E`--`$C43B8E` | not entered directly in this batch | 5 | 0 |
| 38 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 38 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 38 | `$C43B20`--`$C43B2C` | `$C43B1A / $00C43B1E` | 4 | 0 |
| 38 | `$C43A7E`--`$C43A9E` | `$C43A78 / $00C43AF0` | 9 | 0 |
| 38 | `$C43AA4`--`$C43AC4` | not entered directly in this batch | 9 | 0 |
| 38 | `$C43ACA`--`$C43AEA` | not entered directly in this batch | 9 | 0 |
| 38 | `$C439EE`--`$C43A06` | `$C439E8 / $00C43A44` | 7 | 0 |
| 39 | `$C43A0C`--`$C43A1C` | not entered directly in this batch | 5 | 0 |
| 39 | `$C43A22`--`$C43A3E` | not entered directly in this batch | 8 | 0 |
| 42 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 42 | `$C42E44`--`$C42E4C` | `$C42E3E / $00C42E42` | 3 | 0 |
| 42 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 43 | `$C42D2E`--`$C42D62` | `$C42D28 / $00C42D2C` | 14 | 1 |
| 43 | `$C42D6A`--`$C42D76` | not entered directly in this batch | 4 | 1 |
| 43 | `$C42D7E`--`$C42D8A` | not entered directly in this batch | 4 | 1 |
| 44 | `$C42D92`--`$C42DAA` | not entered directly in this batch | 7 | 1 |
| 44 | `$C42DB2`--`$C42DBE` | not entered directly in this batch | 4 | 1 |
| 44 | `$C42E04`--`$C42E14` | `$C42DFC / $00C42E00` | 5 | 0 |
| 44 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 44 | `$C42E20`--`$C42E38` | `$C42E1A / $00C42E1E` | 7 | 0 |
| 44 | `$C42DCA`--`$C42DF6` | `$C42DC4 / $00C42DC8` | 12 | 0 |
| 44 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 44 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 44 | `$C42E58`--`$C42E64` | `$C42E52 / $00C42E56` | 4 | 0 |
| 44 | `$C440C2`--`$C440DA` | `$C440BC / $00C440C0` | 7 | 1 |
| 45 | `$C440E0`--`$C44108` | not entered directly in this batch | 11 | 1 |
| 45 | `$C4410E`--`$C4412A` | not entered directly in this batch | 8 | 1 |
| 45 | `$C44130`--`$C44144` | not entered directly in this batch | 6 | 1 |
| 45 | `$C4414A`--`$C4415A` | not entered directly in this batch | 5 | 1 |
| 45 | `$C440A4`--`$C440B4` | `$C4409E / $00C440A2` | 5 | 1 |
| 46 | `$C43FD6`--`$C44006` | `$C43FD0 / $00C43FD4` | 13 | 1 |
| 46 | `$C4400C`--`$C44034` | not entered directly in this batch | 11 | 1 |
| 46 | `$C4403A`--`$C44046` | not entered directly in this batch | 4 | 1 |
| 46 | `$C43F64`--`$C43F78` | `$C43F5E / $00C43FAE` | 6 | 1 |
| 47 | `$C43F7E`--`$C43F86` | not entered directly in this batch | 3 | 1 |
| 47 | `$C43F8C`--`$C43FA8` | not entered directly in this batch | 8 | 1 |
| 47 | `$C43ED0`--`$C43EF0` | `$C43ECA / $00C43F3A` | 9 | 1 |
| 47 | `$C43EF6`--`$C43F16` | not entered directly in this batch | 9 | 1 |
| 47 | `$C43F1C`--`$C43F34` | not entered directly in this batch | 7 | 1 |
| 48 | `$C43D20`--`$C43D2C` | `$C43D1A / $00C43D1E` | 4 | 1 |
| 48 | `$C43CC4`--`$C43CF4` | `$C43CBE / $00C43CFA` | 13 | 1 |
| 48 | `$C43BB2`--`$C43BCA` | `$C43BAC / $00C43C70` | 7 | 1 |
| 48 | `$C43BD0`--`$C43BF4` | not entered directly in this batch | 10 | 0 |

The JSON companion retains every exact consumed signed pair and its static address.

## Direct packet stream selections

`$C2AF40` chooses the inline source at `header + 4` when `D7` is zero, otherwise the header longword.

| Frame | Header | Inline stream | Header pointer | `D7` | Selected stream | Route |
| ---: | --- | --- | --- | ---: | --- | --- |
| 25 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 25 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 25 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 25 | `$C43A78` | `$C43A7C` | `$C43AF0` | 0 | `$C43A7C` | inline |
| 25 | `$C439E8` | `$C439EC` | `$C43A44` | 0 | `$C439EC` | inline |
| 29 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 29 | `$C42E3E` | `$C42E42` | `$C42E42` | 0 | `$C42E42` | inline |
| 29 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 29 | `$C42D28` | `$C42D2C` | `$C42D2C` | 0 | `$C42D2C` | inline |
| 31 | `$C42DFC` | `$C42E00` | `$C42E00` | 0 | `$C42E00` | inline |
| 31 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 31 | `$C42E1A` | `$C42E1E` | `$C42E1E` | 0 | `$C42E1E` | inline |
| 31 | `$C42DC4` | `$C42DC8` | `$C42DC8` | 0 | `$C42DC8` | inline |
| 31 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 31 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 31 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 31 | `$C440BC` | `$C440C0` | `$C440C0` | 0 | `$C440C0` | inline |
| 32 | `$C4409E` | `$C440A2` | `$C440A2` | 0 | `$C440A2` | inline |
| 32 | `$C43FD0` | `$C43FD4` | `$C43FD4` | 0 | `$C43FD4` | inline |
| 33 | `$C43F5E` | `$C43F62` | `$C43FAE` | 0 | `$C43F62` | inline |
| 33 | `$C43ECA` | `$C43ECE` | `$C43F3A` | 0 | `$C43ECE` | inline |
| 34 | `$C43D1A` | `$C43D1E` | `$C43D1E` | 0 | `$C43D1E` | inline |
| 34 | `$C43CBE` | `$C43CC2` | `$C43CFA` | 0 | `$C43CC2` | inline |
| 35 | `$C43BAC` | `$C43BB0` | `$C43C70` | 0 | `$C43BB0` | inline |
| 36 | `$C4424A` | `$C4424E` | `$C4424E` | 0 | `$C4424E` | inline |
| 36 | `$C4420C` | `$C44210` | `$C44210` | 0 | `$C44210` | inline |
| 36 | `$C4417A` | `$C4417E` | `$C4417E` | 0 | `$C4417E` | inline |
| 36 | `$C44160` | `$C44164` | `$C44164` | 0 | `$C44164` | inline |
| 36 | `$C4407C` | `$C44080` | `$C44080` | 0 | `$C44080` | inline |
| 37 | `$C4404C` | `$C44050` | `$C44050` | 0 | `$C44050` | inline |
| 37 | `$C43E24` | `$C43E28` | `$C43E9A` | 0 | `$C43E28` | inline |
| 38 | `$C43D1A` | `$C43D1E` | `$C43D1E` | 0 | `$C43D1E` | inline |
| 38 | `$C43B34` | `$C43B38` | `$C43B94` | 0 | `$C43B38` | inline |
| 38 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 38 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 38 | `$C43B1A` | `$C43B1E` | `$C43B1E` | 0 | `$C43B1E` | inline |
| 38 | `$C43A78` | `$C43A7C` | `$C43AF0` | 0 | `$C43A7C` | inline |
| 38 | `$C439E8` | `$C439EC` | `$C43A44` | 0 | `$C439EC` | inline |
| 42 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 42 | `$C42E3E` | `$C42E42` | `$C42E42` | 0 | `$C42E42` | inline |
| 42 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 43 | `$C42D28` | `$C42D2C` | `$C42D2C` | 0 | `$C42D2C` | inline |
| 44 | `$C42DFC` | `$C42E00` | `$C42E00` | 0 | `$C42E00` | inline |
| 44 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 44 | `$C42E1A` | `$C42E1E` | `$C42E1E` | 0 | `$C42E1E` | inline |
| 44 | `$C42DC4` | `$C42DC8` | `$C42DC8` | 0 | `$C42DC8` | inline |
| 44 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 44 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 44 | `$C42E52` | `$C42E56` | `$C42E56` | 0 | `$C42E56` | inline |
| 44 | `$C440BC` | `$C440C0` | `$C440C0` | 0 | `$C440C0` | inline |
| 45 | `$C4409E` | `$C440A2` | `$C440A2` | 0 | `$C440A2` | inline |
| 46 | `$C43FD0` | `$C43FD4` | `$C43FD4` | 0 | `$C43FD4` | inline |
| 46 | `$C43F5E` | `$C43F62` | `$C43FAE` | 0 | `$C43F62` | inline |
| 47 | `$C43ECA` | `$C43ECE` | `$C43F3A` | 0 | `$C43ECE` | inline |
| 48 | `$C43D1A` | `$C43D1E` | `$C43D1E` | 0 | `$C43D1E` | inline |
| 48 | `$C43CBE` | `$C43CC2` | `$C43CFA` | 0 | `$C43CC2` | inline |
| 48 | `$C43BAC` | `$C43BB0` | `$C43C70` | 0 | `$C43BB0` | inline |

The same coordinate pairs are available as grouped `l` primitives in the [OBJ inspection export](../exports/run003_m_map_static_pair_batches.obj) for run003 only. It writes `(source_x, 0, source_y)` solely as a viewer convention, with no faces, path closure, global placement, or game-axis semantics inferred.
