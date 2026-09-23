# Live map packet selector samples

Metric and alternate-mode values are live frame values read at C2AF40. They establish the immediate selector state only; their higher-level physical meaning requires separate dataflow evidence.

- Inline route samples: 20
- Alternate route samples: 20

- Instruction budget: 300000
- Distinct headers: 8

| Instruction | Header | Metric | Alternate mode | `D7` | Route | Selected stream |
| ---: | --- | ---: | ---: | ---: | --- | --- |
| 31243 | `$C42D28` | 1113 | 0 | 0 | inline | `$C42D2C` |
| 35356 | `$C42DFC` | 1113 | 0 | 0 | inline | `$C42E00` |
| 35467 | `$C42E1A` | 1113 | 0 | 0 | inline | `$C42E1E` |
| 36789 | `$C42DC4` | 1113 | 0 | 0 | inline | `$C42DC8` |
| 37685 | `$C43F5E` | 1113 | 1 | 55784 | alternate | `$C43FAE` |
| 39609 | `$C43ECA` | 1113 | 1 | 1 | alternate | `$C43F3A` |
| 41667 | `$C43CBE` | 1113 | 1 | 55784 | alternate | `$C43CFA` |
| 43915 | `$C43BAC` | 1113 | 1 | 1 | alternate | `$C43C70` |
| 78981 | `$C42D28` | 1118 | 0 | 0 | inline | `$C42D2C` |
| 83109 | `$C42DFC` | 1118 | 0 | 0 | inline | `$C42E00` |
| 83220 | `$C42E1A` | 1118 | 0 | 0 | inline | `$C42E1E` |
| 84552 | `$C42DC4` | 1118 | 0 | 0 | inline | `$C42DC8` |
| 85448 | `$C43F5E` | 1118 | 1 | 32028 | alternate | `$C43FAE` |
| 87392 | `$C43ECA` | 1118 | 1 | 1 | alternate | `$C43F3A` |
| 88905 | `$C43CBE` | 1118 | 1 | 32028 | alternate | `$C43CFA` |
| 91669 | `$C43BAC` | 1118 | 1 | 1 | alternate | `$C43C70` |
| 131614 | `$C42D28` | 1124 | 0 | 0 | inline | `$C42D2C` |
| 135727 | `$C42DFC` | 1124 | 0 | 0 | inline | `$C42E00` |
| 135838 | `$C42E1A` | 1124 | 0 | 0 | inline | `$C42E1E` |
| 137160 | `$C42DC4` | 1124 | 0 | 0 | inline | `$C42DC8` |
| 138056 | `$C43F5E` | 1124 | 1 | 8272 | alternate | `$C43FAE` |
| 139980 | `$C43ECA` | 1124 | 1 | 1 | alternate | `$C43F3A` |
| 142046 | `$C43CBE` | 1124 | 1 | 8272 | alternate | `$C43CFA` |
| 144435 | `$C43BAC` | 1124 | 1 | 1 | alternate | `$C43C70` |
| 182850 | `$C42D28` | 1129 | 0 | 0 | inline | `$C42D2C` |
| 186427 | `$C42DFC` | 1129 | 0 | 0 | inline | `$C42E00` |
| 186538 | `$C42E1A` | 1129 | 0 | 0 | inline | `$C42E1E` |
| 187864 | `$C42DC4` | 1129 | 0 | 0 | inline | `$C42DC8` |
| 188760 | `$C43F5E` | 1129 | 1 | 50052 | alternate | `$C43FAE` |
| 190684 | `$C43ECA` | 1129 | 1 | 1 | alternate | `$C43F3A` |
| 192738 | `$C43CBE` | 1129 | 1 | 50052 | alternate | `$C43CFA` |
| 195153 | `$C43BAC` | 1129 | 1 | 1 | alternate | `$C43C70` |
| 257783 | `$C42D28` | 1135 | 0 | 0 | inline | `$C42D2C` |
| 261884 | `$C42DFC` | 1135 | 0 | 0 | inline | `$C42E00` |
| 261995 | `$C42E1A` | 1135 | 0 | 0 | inline | `$C42E1E` |
| 263313 | `$C42DC4` | 1135 | 0 | 0 | inline | `$C42DC8` |
| 264209 | `$C43F5E` | 1135 | 1 | 26296 | alternate | `$C43FAE` |
| 266133 | `$C43ECA` | 1135 | 1 | 1 | alternate | `$C43F3A` |
| 268191 | `$C43CBE` | 1135 | 1 | 26296 | alternate | `$C43CFA` |
| 270583 | `$C43BAC` | 1135 | 1 | 1 | alternate | `$C43C70` |
