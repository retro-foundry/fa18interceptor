# Live map packet selector samples

Metric and alternate-mode values are live frame values read at C2AF40. They establish the immediate selector state only; their higher-level physical meaning requires separate dataflow evidence.

- Inline route samples: 56
- Alternate route samples: 0

- Instruction budget: 300000
- Distinct headers: 23

| Instruction | Header | Metric | Alternate mode | `D7` | Route | Selected stream |
| ---: | --- | ---: | ---: | ---: | --- | --- |
| 9345 | `$C42E3E` | 196608 | 0 | 0 | inline | `$C42E42` |
| 9749 | `$C42D28` | 196608 | 0 | 0 | inline | `$C42D2C` |
| 19387 | `$C42DFC` | 196608 | 0 | 0 | inline | `$C42E00` |
| 19864 | `$C42E1A` | 196608 | 0 | 0 | inline | `$C42E1E` |
| 21529 | `$C42DC4` | 196608 | 0 | 0 | inline | `$C42DC8` |
| 22658 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 23025 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 23638 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 24251 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 24710 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 25169 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 25722 | `$C43ECA` | 196608 | 1 | 0 | inline | `$C43ECE` |
| 32938 | `$C43E24` | 196608 | 1 | 0 | inline | `$C43E28` |
| 40853 | `$C43D84` | 196608 | 1 | 0 | inline | `$C43D88` |
| 51076 | `$C43BAC` | 196608 | 1 | 0 | inline | `$C43BB0` |
| 62885 | `$C43B34` | 196608 | 1 | 0 | inline | `$C43B38` |
| 68247 | `$C43A78` | 196608 | 1 | 0 | inline | `$C43A7C` |
| 75672 | `$C439E8` | 196608 | 1 | 0 | inline | `$C439EC` |
| 81044 | `$C4409E` | 196608 | 1 | 0 | inline | `$C440A2` |
| 81967 | `$C4407C` | 196608 | 1 | 0 | inline | `$C44080` |
| 82537 | `$C44066` | 196608 | 1 | 0 | inline | `$C4406A` |
| 82853 | `$C43F5E` | 196608 | 1 | 0 | inline | `$C43F62` |
| 87353 | `$C43D34` | 196608 | 1 | 0 | inline | `$C43D38` |
| 91804 | `$C43CBE` | 196608 | 1 | 0 | inline | `$C43CC2` |
| 95753 | `$C43B1A` | 196608 | 1 | 0 | inline | `$C43B1E` |
| 97081 | `$C439CC` | 196608 | 1 | 0 | inline | `$C439D0` |
| 98253 | `$C439A8` | 196608 | 1 | 0 | inline | `$C439AC` |
| 99732 | `$C43994` | 196608 | 1 | 0 | inline | `$C43998` |
| 143043 | `$C42E3E` | 196608 | 0 | 0 | inline | `$C42E42` |
| 143447 | `$C42D28` | 196608 | 0 | 0 | inline | `$C42D2C` |
| 153045 | `$C42DFC` | 196608 | 0 | 0 | inline | `$C42E00` |
| 153522 | `$C42E1A` | 196608 | 0 | 0 | inline | `$C42E1E` |
| 155182 | `$C42DC4` | 196608 | 0 | 0 | inline | `$C42DC8` |
| 156311 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 156678 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 157291 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 157904 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 158363 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 158822 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 160418 | `$C43ECA` | 196608 | 1 | 0 | inline | `$C43ECE` |
| 167127 | `$C43E24` | 196608 | 1 | 0 | inline | `$C43E28` |
| 174423 | `$C43D84` | 196608 | 1 | 0 | inline | `$C43D88` |
| 184646 | `$C43BAC` | 196608 | 1 | 0 | inline | `$C43BB0` |
| 198181 | `$C43B34` | 196608 | 1 | 0 | inline | `$C43B38` |
| 204101 | `$C43A78` | 196608 | 1 | 0 | inline | `$C43A7C` |
| 211530 | `$C439E8` | 196608 | 1 | 0 | inline | `$C439EC` |
| 216902 | `$C4409E` | 196608 | 1 | 0 | inline | `$C440A2` |
| 217328 | `$C4407C` | 196608 | 1 | 0 | inline | `$C44080` |
| 217898 | `$C44066` | 196608 | 1 | 0 | inline | `$C4406A` |
| 218214 | `$C43F5E` | 196608 | 1 | 0 | inline | `$C43F62` |
| 223219 | `$C43D34` | 196608 | 1 | 0 | inline | `$C43D38` |
| 227606 | `$C43CBE` | 196608 | 1 | 0 | inline | `$C43CC2` |
| 231561 | `$C43B1A` | 196608 | 1 | 0 | inline | `$C43B1E` |
| 232889 | `$C439CC` | 196608 | 1 | 0 | inline | `$C439D0` |
| 234061 | `$C439A8` | 196608 | 1 | 0 | inline | `$C439AC` |
| 235540 | `$C43994` | 196608 | 1 | 0 | inline | `$C43998` |
