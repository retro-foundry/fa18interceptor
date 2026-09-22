# Map segment-68 relative-offset directory

Classification: **traced 2-D map packet selector**.

C2ADC0 doubles the live X index, C2ADC2 shifts the live Y index by four, and C2ADCE reads a word at C42CA8 plus their sum before C2ADD4 adds it to the base. This proves the 8-word row stride and exports the 8x8 prefix used by the observed path. It does not establish absolute world coordinates, cardinal orientation, that every cell is terrain, or LOD.

The exported prefix is `8x8`, rooted at `$C42CA8` with `16`-byte rows. The trace reads 12 unique cells. Two targets account for 59 of the 64 cells; the remaining five targets occur once each.

The same static structure is available as a [colour-coded directory grid](../plots/run003_m_map_segment68_directory.png): colour denotes the recovered static target and white insets identify the twelve cells actually selected in run003. It is a selector/reuse view, not a map-space layout or LOD visualisation.

| y / x | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| ---: | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | `$C42E6A` | `$C42E6A` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` |
| 1 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` |
| 2 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` |
| 3 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E52` | `$C42E52` | `$C42E52` | `$C42E52` |
| 4 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42DC4` | `$C42E1A` | `$C42E52` | `$C42E52` |
| 5 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42DFC` | `$C42D28` | `$C42E52` | `$C42E52` |
| 6 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E3E` | `$C42E52` | `$C42E52` |
| 7 | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E6A` | `$C42E52` | `$C42E52` |

Observed selector accesses: `(4,5)`, `(3,5)`, `(5,4)`, `(4,4)`, `(3,4)`, `(6,6)`, `(6,5)`, `(6,4)`, `(6,3)`, `(5,3)`, `(4,3)`, `(3,3)`.
