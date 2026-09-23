# Static segment-68 streams reachable from traced M-map headers

Classification: **structurally decoded static packet payload rooted at
scenario-observed headers and byte-exact directory targets**. The decoder follows the byte-exact count/threshold
grammar used by `$C2AF46`; it does not claim every reachable stream rendered in
one frame, represents terrain, or has global flight-map placement.

24 direct `$C2AF00` headers plus
2 additional `$C2AF40` selector-sampled
headers and 45 directory-only header targets expose 79 distinct inline/alternate stream starts. Their
complete structural walks contain 817 pair records occupying
3268 payload bytes (52.51% of the
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
| `$C43672` | alternate, inline | 1 | 4 | `$C43684` |
| `$C4368C` | alternate, inline | 1 | 4 | `$C4369E` |
| `$C436A6` | alternate, inline | 1 | 4 | `$C436B8` |
| `$C436C0` | alternate, inline | 1 | 4 | `$C436D2` |
| `$C436DA` | alternate, inline | 1 | 4 | `$C436EC` |
| `$C436F4` | alternate, inline | 1 | 4 | `$C43706` |
| `$C4370E` | alternate, inline | 1 | 4 | `$C43720` |
| `$C43728` | alternate, inline | 1 | 4 | `$C4373A` |
| `$C43742` | alternate, inline | 1 | 4 | `$C43754` |
| `$C4375C` | alternate, inline | 1 | 4 | `$C4376E` |
| `$C43776` | alternate, inline | 1 | 6 | `$C43790` |
| `$C43796` | alternate, inline | 1 | 10 | `$C437C0` |
| `$C437C6` | alternate, inline | 1 | 4 | `$C437D8` |
| `$C437E0` | alternate, inline | 2 | 17 | `$C43828` |
| `$C4382E` | alternate, inline | 1 | 4 | `$C43840` |
| `$C43848` | alternate, inline | 2 | 20 | `$C4389C` |
| `$C438A2` | alternate, inline | 1 | 4 | `$C438B4` |
| `$C438BC` | alternate, inline | 1 | 7 | `$C438DA` |
| `$C438E0` | alternate, inline | 1 | 12 | `$C43912` |
| `$C43918` | alternate, inline | 1 | 4 | `$C4392A` |
| `$C43932` | alternate, inline | 1 | 4 | `$C43944` |
| `$C4394A` | alternate, inline | 1 | 11 | `$C43978` |
| `$C4397E` | alternate, inline | 1 | 4 | `$C43990` |
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
| `$C43FD4` | alternate, inline | 3 | 28 | `$C4404A` |
| `$C44050` | alternate, inline | 1 | 4 | `$C44062` |
| `$C4406A` | alternate, inline | 1 | 3 | `$C44078` |
| `$C44080` | alternate, inline | 2 | 6 | `$C4409C` |
| `$C440A2` | alternate, inline | 1 | 5 | `$C440B8` |
| `$C440C0` | alternate, inline | 5 | 37 | `$C4415E` |
| `$C44164` | alternate, inline | 1 | 4 | `$C44176` |
| `$C4417E` | alternate, inline | 3 | 33 | `$C44208` |
| `$C44210` | alternate, inline | 2 | 13 | `$C44248` |
| `$C4424E` | alternate, inline | 1 | 13 | `$C44284` |
| `$C4428A` | alternate, inline | 1 | 4 | `$C4429C` |
| `$C442A4` | alternate, inline | 1 | 8 | `$C442C6` |
| `$C442CE` | alternate, inline | 2 | 15 | `$C4430E` |
| `$C44314` | alternate, inline | 1 | 10 | `$C4433E` |
| `$C44344` | alternate, inline | 1 | 4 | `$C44356` |
| `$C4435E` | alternate, inline | 2 | 17 | `$C443A6` |
| `$C443AE` | alternate, inline | 2 | 16 | `$C443F2` |
| `$C443F8` | alternate, inline | 1 | 4 | `$C4440A` |
| `$C44412` | alternate, inline | 1 | 4 | `$C44424` |
| `$C4442C` | alternate, inline | 1 | 4 | `$C4443E` |
| `$C44446` | alternate, inline | 1 | 4 | `$C44458` |
| `$C44460` | alternate, inline | 1 | 4 | `$C44472` |
| `$C4447A` | alternate, inline | 1 | 4 | `$C4448C` |
| `$C44494` | alternate, inline | 1 | 4 | `$C444A6` |
| `$C444AE` | alternate, inline | 1 | 4 | `$C444C0` |
| `$C444C8` | alternate, inline | 1 | 4 | `$C444DA` |
| `$C444E2` | alternate, inline | 1 | 4 | `$C444F4` |

A negative non-`$FFFF` prefix is retained as a threshold word because the
renderer clears its sign bit, scales it by four, compares it with the live depth
metric, and then reads the following count. It is not decoded as elevation or
as a terrain LOD distance. A positive prefix is directly the count. All pairs
remain signed two-word source values; depth is computed later by the renderer.

Authority: direct-header inventories generated from sealed traces and the
byte-exact `$C2AEFC-$C2AFF7` reader/stream selector reconstruction.

The exact decoded streams are also available as [raw-pair inspection image]
(../plots/static_m_map_packet_streams.png) and [OBJ line groups]
(../exports/static_m_map_packet_streams.obj). The OBJ writes `(x, 0, y)`
solely as a viewer carrier and does not infer faces or game-space elevation.

## Directory-backed header coverage

A conservative even-address scan finds 71 locations
whose leading longword points inside segment 68 and whose inline and alternate
streams both complete under the exact count/threshold grammar. 71
have dynamic renderer evidence or a non-reject static directory target in the sealed runs.
The wide 32×32 directory supplies a byte-exact producer path for the 45
otherwise unexecuted headers. Therefore no grammar-compatible header remains
unrooted in the current scan; this promotes packet structure, not terrain identity
or evidence that every stream rendered in one draw.

The JSON retains every header and its direct, selector, and directory evidence.
