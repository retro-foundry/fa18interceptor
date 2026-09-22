# Verified adjacent Hunk placements

Authority: original executable `local/extracted/f18_interceptor` compared with
`captures/baseline_menu/slow.bin` by `scripts/map_loaded_segment.py`.

| Segment | Runtime base | Bytes | Non-relocation mismatches |
| ---: | --- | ---: | ---: |
| 41 | `$C34A50` | 2,832 | 0 |
| 42 | `$C35568` | 3,224 | 0 |
| 43 | `$C36208` | 2,068 | 0 |
| 44 | `$C36A28` | 2,024 | 0 |
| 45 | `$C08718` | 724 | 0 |
| 46 | `$C37218` | 1,120 | 0 |
| 47 | `$C37680` | 772 | 0 |
| 48 | `$C0DB50` | 608 | 0 |
| 49 | `$C37990` | 1,512 | 0 |
| 50 | `$C37F80` | 1,124 | 0 |

Segment 48 is now included at `$C0DB50`. The earlier `$C0DB38` candidate was
misaligned by `$18`: comparison at the corrected payload base gives zero
non-relocation mismatches in both `captures/baseline_menu/slow.bin` and the
frame-14,500 bridge checkpoint. Its ten relocation operands all target segment
46; the generated resolver records the resulting relocation closure.
