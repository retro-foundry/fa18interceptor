# Map polygon static-packet inventory

Classification: **bounded immutable input to map-page polygon transforms**.

Each row begins at the traced direct C2AEFC-to-C2AF00 entry with A3 in immutable slow RAM. The listed pairs are only those read at the C2AF9C/C2AF9E fixed-point transform pair before the next direct packet entry. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map.

- Direct `$C2AF00` packet entries: 26
- Entries completing at `$C2AFE2`: 26
- Exact immutable coordinate pairs consumed: 327

## Packets

| Trace frame | Static packet | Raw first longword | Following words | Consumed coordinate-pairs | `$C2FF48` before next packet |
| ---: | --- | --- | --- | ---: | ---: |
| 2 | `$C42DFC` | `$00C42E00` | `['$8400', '$0005']` | 5 | 0 |
| 2 | `$C42E1A` | `$00C42E1E` | `['$0007', '$1100']` | 7 | 1 |
| 2 | `$C42DC4` | `$00C42DC8` | `['$000C', '$1000']` | 12 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C42E52` | `$00C42E56` | `['$0004', '$1100']` | 4 | 0 |
| 2 | `$C43ECA` | `$00C43F3A` | `['$0009', '$0890']` | 25 | 3 |
| 3 | `$C43E24` | `$00C43E9A` | `['$0005', '$1000']` | 26 | 3 |
| 3 | `$C43D84` | `$00C43D88` | `['$0003', '$1000']` | 36 | 3 |
| 4 | `$C43BAC` | `$00C43C70` | `['$0007', '$1000']` | 45 | 5 |
| 5 | `$C43B34` | `$00C43B94` | `['$0008', '$1040']` | 21 | 3 |
| 6 | `$C43A78` | `$00C43AF0` | `['$0009', '$1080']` | 27 | 3 |
| 6 | `$C439E8` | `$00C43A44` | `['$0007', '$1080']` | 20 | 3 |
| 7 | `$C4409E` | `$00C440A2` | `['$0005', '$0000']` | 5 | 0 |
| 7 | `$C4407C` | `$00C44080` | `['$0003', '$1020']` | 6 | 0 |
| 7 | `$C44066` | `$00C4406A` | `['$0003', '$0CF0']` | 3 | 0 |
| 7 | `$C43F5E` | `$00C43FAE` | `['$0006', '$1000']` | 17 | 2 |
| 7 | `$C43D34` | `$00C43D38` | `['$0005', '$0EF0']` | 17 | 2 |
| 8 | `$C43CBE` | `$00C43CFA` | `['$000D', '$1080']` | 13 | 1 |
| 8 | `$C43B1A` | `$00C43B1E` | `['$0004', '$1080']` | 4 | 1 |
| 8 | `$C439CC` | `$00C439D0` | `['$0004', '$1080']` | 4 | 1 |
| 8 | `$C439A8` | `$00C439AC` | `['$0007', '$0890']` | 7 | 1 |
| 8 | `$C43994` | `$00C43998` | `['$0003', '$0580']` | 3 | 5 |

The JSON companion retains every exact consumed signed pair and its static address.
