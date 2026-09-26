# run075 frame 559 `$C2F688` call set

The frame559 transition reaches five `$C2F688` calls during frames 561
through 565. Their entry coordinate pairs are:

| Engine frame | `D0.w` | `D1.w` |
| ---: | ---: | ---: |
| 561 | 140 | 105 |
| 562 | 52 | 106 |
| 563 | 297 | 106 |
| 564 | 231 | 109 |
| 565 | 101 | 101 |

The calls share the same four logical plane lane bases and vary the selected
word offset and mask packet. This is the complete `$C2F688` call set in the
frame559 transition window; the surrounding `$C2FF48` coordinate pass is the
producer of these points.

Authority: the five reports in
`build/run075_c2f688_559_hit0/report.json` through
`build/run075_c2f688_559_hit4/report.json`, captured from the canonical
run075 restore and playback with the breakpoint armed at frame 558.
