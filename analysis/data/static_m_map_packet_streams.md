# Static segment-68 streams reachable from traced M-map headers

Classification: **structurally decoded static packet payload rooted at
scenario-observed headers**. The decoder follows the byte-exact count/threshold
grammar used by `$C2AF46`; it does not claim every reachable stream rendered in
one frame, represents terrain, or has global flight-map placement.

24 direct headers observed across the listed sealed M-map
inventories expose 32 distinct inline/alternate stream starts. Their
complete structural walks contain 424 pair records occupying
1696 payload bytes (27.25% of the
segment). This expands static *reachable-format* coverage beyond dynamically
consumed pairs; it is deliberately reported separately from trace coverage.

| Stream | roles | batches | pairs | terminator |
| --- | --- | ---: | ---: | --- |
| `$C42D2C` | alternate, inline | 5 | 33 | `$C42DC2` |
| `$C42DC8` | alternate, inline | 1 | 12 | `$C42DFA` |
| `$C42E00` | alternate, inline | 1 | 5 | `$C42E18` |
| `$C42E1E` | alternate, inline | 1 | 7 | `$C42E3C` |
| `$C42E42` | alternate, inline | 1 | 3 | `$C42E50` |
| `$C42E56` | alternate, inline | 1 | 4 | `$C42E68` |
| `$C43998` | alternate, inline | 1 | 3 | `$C439A6` |
| `$C439AC` | alternate, inline | 1 | 7 | `$C439CA` |
| `$C439D0` | alternate, inline | 1 | 4 | `$C439E2` |
| `$C439EC` | inline | 3 | 20 | `$C43A42` |
| `$C43A44` | alternate | 1 | 12 | `$C43A76` |
| `$C43A7C` | inline | 3 | 27 | `$C43AEE` |
| `$C43AF0` | alternate | 2 | 9 | `$C43B18` |
| `$C43B1E` | alternate, inline | 1 | 4 | `$C43B30` |
| `$C43B38` | inline | 3 | 21 | `$C43B92` |
| `$C43B94` | alternate | 1 | 5 | `$C43BAA` |
| `$C43BB0` | inline | 5 | 45 | `$C43C6E` |
| `$C43C70` | alternate | 2 | 18 | `$C43CBC` |
| `$C43CC2` | inline | 1 | 13 | `$C43CF8` |
| `$C43CFA` | alternate | 1 | 7 | `$C43D18` |
| `$C43D1E` | alternate, inline | 1 | 4 | `$C43D30` |
| `$C43D38` | alternate, inline | 3 | 17 | `$C43D82` |
| `$C43D88` | alternate, inline | 5 | 36 | `$C43E22` |
| `$C43E28` | inline | 4 | 26 | `$C43E98` |
| `$C43E9A` | alternate | 1 | 11 | `$C43EC8` |
| `$C43ECE` | inline | 3 | 25 | `$C43F38` |
| `$C43F3A` | alternate | 1 | 8 | `$C43F5C` |
| `$C43F62` | inline | 3 | 17 | `$C43FAC` |
| `$C43FAE` | alternate | 2 | 7 | `$C43FCE` |
| `$C4406A` | alternate, inline | 1 | 3 | `$C44078` |
| `$C44080` | alternate, inline | 2 | 6 | `$C4409C` |
| `$C440A2` | alternate, inline | 1 | 5 | `$C440B8` |

A negative non-`$FFFF` prefix is retained as a threshold word because the
renderer clears its sign bit, scales it by four, compares it with the live depth
metric, and then reads the following count. It is not decoded as elevation or
as a terrain LOD distance. A positive prefix is directly the count. All pairs
remain signed two-word source values; depth is computed later by the renderer.

Authority: direct-header inventories generated from sealed traces and the
byte-exact `$C2AEFC-$C2AFF7` reader/stream selector reconstruction.

## Grammar-compatible header candidates

A conservative even-address scan finds 71 locations
whose leading longword points inside segment 68 and whose inline and alternate
streams both complete under the exact count/threshold grammar. 24
are direct renderer entries in the sealed traces. The remaining candidates are
not promoted to packet headers: coordinate payload can coincidentally satisfy a
small grammar, so dynamic entry or a static producer reference is still required.

The JSON retains every candidate and marks direct-entry status for use as a
targeted trace list rather than as an unverified map export.
