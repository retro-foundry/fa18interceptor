# run075 frame 200 menu display source

## Evidence

`captures/baseline_menu/screen.png` is byte-for-byte identical to the host
oracle image `build/port_run075_demo_oracle/200.png`. The game image is the
320x200 region at host coordinates `x=40..679`, `y=16..215`, with each game
pixel doubled horizontally. Sampling the left pixel of each pair gives the
native image used below.

The corresponding baseline Chip-RAM snapshot contains three exact 8,000-byte
plane images. Each plane is 40 bytes per row and 200 rows:

| Native plane bit | Chip-RAM source | Evidence |
| ---: | ---: | --- |
| 0 | `$012BC0` | exact 8,000-byte match |
| 1 | `$014B00` | exact 8,000-byte match |
| 2 | `$016A40` | exact 8,000-byte match |
| 3 | `$018980` | all zero for this frame |

The search was performed against every possible subset of the six displayed
RGB values. The matching colour subsets are:

| Source RGB | RGB4 display value | Native index | Plane bits |
| --- | ---: | ---: | --- |
| `(0,0,0)` | `$000` | 0 | `0000` |
| `(85,85,85)` | `$555` | 2 | `0010` |
| `(85,85,187)` | `$55B` | 5 | `0101` |
| `(34,136,34)` | `$282` | 4 | `0100` |
| `(170,34,0)` | `$A20` | 1 | `0001` |
| `(153,102,17)` | `$961` | 3 | `0011` |

The index assignments are the unique assignments consistent with all three
8,000-byte plane matches. They establish the frame-200 indexed chunky image
and its six RGB4 palette entries without retaining Amiga addresses in the
runtime model.

## Native port boundary

The native implementation should represent this as a `FA18IndexedFrameBuffer`
plus `FA18Palette` data. The plane addresses above are provenance for the
fixture only. They are not fields in the C runtime. The menu text producer and
glyph layout still need to be connected to this frame before frame 201 is
unlocked.

## Reproduction

The comparison used `captures/baseline_menu/chip.bin` and
`captures/baseline_menu/screen.png`. The six sampled colours were mapped to
RGB4 by dividing each 8-bit channel by 17. Matching each candidate plane was
done by packing the corresponding colour subset MSB-first, row by row, and
searching the Chip-RAM snapshot for the complete 8,000-byte result.
