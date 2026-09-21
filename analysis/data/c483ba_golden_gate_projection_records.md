# `$C483BA`: Golden Gate-frame consecutive projection records

Classification: **runtime-backed bridge-frame geometry packet**. The records
are not yet assigned to named bridge members.

## Selection evidence

In `build/run031_frame12000_golden_gate_c1f6f8_probe/`, live `$C211DC` entry
has `A2=$C35734` and `A5=$C355A0`. Its header and relative offset decode as:

```text
header   = $0D01  (count 13, low-six-bit selector 1)
offset   = $002A
base     = $C48390 + $002A = $C483BA
```

`$C211DC` copies each consecutive six-word record to `$C4C592` and invokes
`$C2EE4A`; the trace observes all thirteen calls in the user-identified Golden
Gate frame.

## Raw records

Each row is the six big-endian words copied by one loop iteration.

| Index | Words |
| ---: | --- |
| 0 | `027D 0081 0026 027D 00A7 0023` |
| 1 | `02CE 0047 FF9F 02CF 00A0 FF97` |
| 2 | `031F 000E FF19 0321 0098 FF0B` |
| 3 | `0371 FFD5 FE92 0373 00B2 FE7C` |
| 4 | `03C2 FF9C FE0C 03C5 00CC FDEE` |
| 5 | `0413 FF62 FD85 0417 00E6 FD5F` |
| 6 | `0464 FF29 FCFF 0468 0124 FCCD` |
| 7 | `04B6 FEF0 FC78 04BA 0163 FC3B` |
| 8 | `0507 FEB7 FBF2 050C 01A1 FBAA` |
| 9 | `0558 FE7D FB6B 055B 00C4 FB35` |
| 10 | `05A9 FE44 FAE5 05AA FFE8 FAC0` |
| 11 | `05FB FE0B FA5E 05FA FF0C FA4B` |
| 12 | `064C FDD2 F9D8 064B FE52 F9CE` |

The ordered, smoothly changing values support a coherent geometry sequence,
but no coordinate-axis or bridge-component semantics are inferred here.
