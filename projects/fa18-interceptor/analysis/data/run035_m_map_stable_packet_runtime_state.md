# Live map packet selector samples

Metric and alternate-mode values are live frame values read at C2AF40. They establish the immediate selector state only; their higher-level physical meaning requires separate dataflow evidence.

- Inline route samples: 31
- Alternate route samples: 0

| Instruction | Header | Metric | Alternate mode | `D7` | Route | Selected stream |
| ---: | --- | ---: | ---: | ---: | --- | --- |
| 46953 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 47320 | `$C42E3E` | 196608 | 0 | 0 | inline | `$C42E42` |
| 47678 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 48855 | `$C42D28` | 196608 | 0 | 0 | inline | `$C42D2C` |
| 59125 | `$C42DFC` | 196608 | 0 | 0 | inline | `$C42E00` |
| 59556 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 60165 | `$C42E1A` | 196608 | 0 | 0 | inline | `$C42E1E` |
| 61417 | `$C42DC4` | 196608 | 0 | 0 | inline | `$C42DC8` |
| 62638 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 63097 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 63556 | `$C42E52` | 196608 | 0 | 0 | inline | `$C42E56` |
| 64109 | `$C43F5E` | 196608 | 1 | 0 | inline | `$C43F62` |
| 69584 | `$C43ECA` | 196608 | 1 | 0 | inline | `$C43ECE` |
| 76118 | `$C43E24` | 196608 | 1 | 0 | inline | `$C43E28` |
| 83480 | `$C43CBE` | 196608 | 1 | 0 | inline | `$C43CC2` |
| 86872 | `$C43BAC` | 196608 | 1 | 0 | inline | `$C43BB0` |
| 100842 | `$C43B34` | 196608 | 1 | 0 | inline | `$C43B38` |
| 106218 | `$C43B1A` | 196608 | 1 | 0 | inline | `$C43B1E` |
| 107510 | `$C43A78` | 196608 | 1 | 0 | inline | `$C43A7C` |
| 114991 | `$C439E8` | 196608 | 1 | 0 | inline | `$C439EC` |
| 120354 | `$C440BC` | 196608 | 1 | 0 | inline | `$C440C0` |
| 124673 | `$C4409E` | 196608 | 1 | 0 | inline | `$C440A2` |
| 126082 | `$C4407C` | 196608 | 1 | 0 | inline | `$C44080` |
| 127964 | `$C43FD0` | 196608 | 1 | 0 | inline | `$C43FD4` |
| 135702 | `$C43D84` | 196608 | 1 | 0 | inline | `$C43D88` |
| 143257 | `$C43D1A` | 196608 | 1 | 0 | inline | `$C43D1E` |
| 144550 | `$C43B1A` | 196608 | 1 | 0 | inline | `$C43B1E` |
| 145879 | `$C439CC` | 196608 | 1 | 0 | inline | `$C439D0` |
| 146342 | `$C439CC` | 196608 | 1 | 0 | inline | `$C439D0` |
| 146805 | `$C439A8` | 196608 | 1 | 0 | inline | `$C439AC` |
| 147502 | `$C43994` | 196608 | 1 | 0 | inline | `$C43998` |
