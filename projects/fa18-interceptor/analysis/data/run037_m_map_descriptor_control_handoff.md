# M-map descriptor-to-control handoff

Classification: **cross-capture scenario-correlated template/placement-to-control-stream ownership**. This is not yet single-execution mesh, terrain-cell, or pixel ownership.

The trace reaches `$C1CC70` **17** times for **17** distinct descriptors. Every observed descriptor is also in the independently captured template-placement inventory. `$C1CC70` reads each descriptor's `+8` longword and writes it to `$C45A36`, the input that `$C1F6F8` loads as its control-stream pointer.

The same control trace enters `$C1F6F8` **6** times. For each entry, `$C1CC70` stores the replay-correlated descriptor's `+8` field, `$C1EF10` publishes the cursor, and `$C1F70E` loads that exact published value into `A5`. `A1` at walker entry is a separate cursor, not the stream pointer. The placement inventory is a separate sealed capture, so this does not yet prove a literal single-execution record-to-mesh chain.

| Frame | descriptor | `+8` field written to `$C45A36` | descriptor in template-placement inventory |
| ---: | --- | --- | --- |
| 5698 | $C22714 | $C35BD0 | yes |
| 5699 | $C22354 | $C4466E | yes |
| 5699 | $C22408 | $C35568 | yes |
| 5699 | $C2241C | $C355A0 | yes |
| 5699 | $C22804 | $C44732 | yes |
| 5699 | $C22584 | $C36212 | yes |
| 5699 | $C22728 | $C36208 | yes |
| 5699 | $C2232C | $C445DC | yes |
| 5699 | $C225FC | $C37EA6 | yes |
| 5699 | $C22890 | $C44970 | yes |
| 5699 | $C22A70 | $C4545C | yes |
| 5699 | $C224D0 | $C3B6A6 | yes |
| 5699 | $C22908 | $C44EF6 | yes |
| 5699 | $C22958 | $C44CE4 | yes |
| 5699 | $C22930 | $C44C10 | yes |
| 5699 | $C22A84 | $C454F4 | yes |
| 5699 | $C22A34 | $C45294 | yes |

| Walker frame | static template -> runtime placement `(X,Y,Z)` | descriptor field | cursor-published stream | `A5` after walker load | exact match |
| ---: | --- | --- | --- | --- |
| 5699 | $C42707 -> $C4EC02 (0, 0, -328) | $C35BD0 | $C35BD2 | $C35BD2 | yes |
| 5699 | $C4264D -> $C4ED22 (460, 0, 172) | $C35568 | $C35598 | $C35598 | yes |
| 5699 | $C42653 -> $C4ED3A (460, 0, 148) | $C355A0 | $C355D0 | $C355D0 | yes |
| 5699 | $C4266B -> $C4ED9A (272, 0, 146) | $C36212 | $C36214 | $C36214 | yes |
| 5699 | $C42671 -> $C4EDB2 (272, 0, 146) | $C36208 | $C36210 | $C36210 | yes |
| 5699 | $C42789 -> $C4F0FA (368, 0, -144) | $C3B6A6 | $C3B6AE | $C3B6AE | yes |

The placement inventory and control trace are separate sealed run037 captures. Their matching static template, runtime descriptor, and descriptor +8 field make this a scenario-correlated instance-class join. Within the control trace, each listed walker has a proven immediate C1CC70 store, C1EE14 cursor publication, and A5 load. A single uninterrupted trace from that exact runtime placement record through its mesh primitives is still required for literal per-instance mesh ownership.
