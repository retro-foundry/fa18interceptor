# M-map descriptor-to-control handoff

Classification: **same-map-mode descriptor-to-control dataflow**. This is not mesh, terrain-cell, or pixel ownership.

The trace reaches `$C1CC70` **17** times for **17** distinct descriptors. Every observed descriptor is also in the independently captured template-placement inventory. `$C1CC70` reads each descriptor's `+8` longword and writes it to `$C45A36`, the input that `$C1F6F8` loads as its control-stream pointer.

The same trace enters `$C1F6F8` **6** times with 5 distinct entry values. These are a scenario-level renderer-control observation; scheduler/order state prevents assigning each walker entry to one preceding placement.

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

C1CC70 stores a descriptor's third longword to C45A36. C1F6F8 later loads C45A36 into A5. The trace establishes descriptor-to-control activity in the same map mode, but does not assign an individual placement to a control-walker invocation, pixel, model, or terrain cell.
