# M-map descriptor-to-control handoff

Classification: **single-trace placement-to-control-stream ownership**. This is not yet per-primitive mesh, terrain-cell, or pixel ownership.

The trace reaches `$C1CC70` **49** times for **44** distinct descriptors. Every observed descriptor is also present in this trace's template-placement inventory. `$C1CC70` reads each descriptor's `+8` longword and writes it to `$C45A36`, the input that `$C1F6F8` loads as its control-stream pointer.

The same trace enters `$C1F6F8` **6** times. For each entry, the exact earlier placement record supplies the descriptor, `$C1CC70` stores its `+8` field, `$C1EF10` publishes the cursor, and `$C1F70E` loads that exact published value into `A5`. `A1` at walker entry is a separate cursor, not the stream pointer.

| Frame | descriptor | `+8` field written to `$C45A36` | descriptor in template-placement inventory |
| ---: | --- | --- | --- |
| 5729 | $C227B4 | $C0DD30 | yes |
| 5729 | $C223E0 | $C08928 | yes |
| 5730 | $C22714 | $C35BD0 | yes |
| 5730 | $C2237C | $C44812 | yes |
| 5730 | $C22390 | $C4483C | yes |
| 5730 | $C22354 | $C4466E | yes |
| 5730 | $C22368 | $C4477C | yes |
| 5730 | $C22408 | $C35568 | yes |
| 5730 | $C2241C | $C355A0 | yes |
| 5730 | $C22804 | $C44732 | yes |
| 5730 | $C22818 | $C447C6 | yes |
| 5730 | $C22318 | $C445A2 | yes |
| 5730 | $C22584 | $C36212 | yes |
| 5730 | $C22728 | $C36208 | yes |
| 5730 | $C227F0 | $C44612 | yes |
| 5730 | $C2232C | $C445DC | yes |
| 5730 | $C2232C | $C445DC | yes |
| 5730 | $C22700 | $C3B4F8 | yes |
| 5730 | $C2232C | $C445DC | yes |
| 5730 | $C227F0 | $C44612 | yes |
| 5730 | $C225FC | $C37EA6 | yes |
| 5730 | $C225E8 | $C37E56 | yes |
| 5730 | $C228CC | $C44A3A | yes |
| 5730 | $C228E0 | $C44AA4 | yes |
| 5730 | $C229D0 | $C45026 | yes |
| 5730 | $C22890 | $C44970 | yes |
| 5730 | $C22A5C | $C453C4 | yes |
| 5730 | $C22A70 | $C4545C | yes |
| 5730 | $C224D0 | $C3B6A6 | yes |
| 5731 | $C22A48 | $C4532C | yes |
| 5731 | $C229A8 | $C44E8C | yes |
| 5731 | $C22908 | $C44EF6 | yes |
| 5731 | $C2287C | $C448B6 | yes |
| 5731 | $C22994 | $C44E22 | yes |
| 5731 | $C22944 | $C44C7A | yes |
| 5731 | $C22958 | $C44CE4 | yes |
| 5731 | $C2296C | $C44D4E | yes |
| 5731 | $C22980 | $C44DB8 | yes |
| 5731 | $C2291C | $C44B78 | yes |
| 5731 | $C22930 | $C44C10 | yes |
| 5731 | $C229BC | $C44F8E | yes |
| 5731 | $C22A98 | $C4558C | yes |
| 5731 | $C22A84 | $C454F4 | yes |
| 5731 | $C22700 | $C3B4F8 | yes |
| 5731 | $C22700 | $C3B4F8 | yes |
| 5731 | $C22A34 | $C45294 | yes |
| 5731 | $C229F8 | $C45128 | yes |
| 5731 | $C22A0C | $C45192 | yes |
| 5731 | $C22A20 | $C451FC | yes |

| Walker frame | static template -> runtime placement `(X,Y,Z)` | descriptor field | cursor-published stream | `A5` after walker load | exact match |
| ---: | --- | --- | --- | --- |
| 5730 | $C42707 -> $C4EC02 (0, 0, -328) | $C35BD0 | $C35BD2 | $C35BD2 | yes |
| 5730 | $C4264D -> $C4ED22 (460, 0, 172) | $C35568 | $C35598 | $C35598 | yes |
| 5730 | $C42653 -> $C4ED3A (460, 0, 148) | $C355A0 | $C355D0 | $C355D0 | yes |
| 5730 | $C4266B -> $C4ED9A (272, 0, 146) | $C36212 | $C36214 | $C36214 | yes |
| 5730 | $C42671 -> $C4EDB2 (272, 0, 146) | $C36208 | $C36210 | $C36210 | yes |
| 5730 | $C42789 -> $C4F0FA (368, 0, -144) | $C3B6A6 | $C3B6AE | $C3B6AE | yes |

The placement inventory and control trace are the same uninterrupted capture. For each listed walker, the exact placement record supplies the descriptor whose +8 field is stored at C1CC70, cursor-published at C1EF10, and loaded into A5 at C1F70E. This proves placement-to-control-stream execution, but a bounded source-to-submission association is still required for per-primitive mesh ownership.
