# run075 frame 584 `$C330FE` HUD lane

The canonical run075 replay reaches `$C330FE` at engine frame 584. The
return bounded trace starts with:

| Input | Value | Meaning established by the routine |
| --- | ---: | --- |
| `D1` | `$00019C74` | destination longword stream |
| `D2` | `$FBFA` | packed source shift and mode input |
| `D3` | `$F000` | set form selected by the high nibble |
| `D4` | `$00C3D897` | glyph byte stream / lane source address |
| `D7` | `$0142` | row and blit-size input |

The first loop derives `D6 = (D7 >> 6) - 1 = 4`, so the `DBRA` loop performs
five iterations in this bounded entry. Each iteration advances the byte stream
by one and the longword stream by `$28`. The trace reads source bytes beginning
at `$C3D897` and writes the strided destination beginning at `$019C74` after
the caller has established the lane addresses. The mask algebra is the existing
`fa18_apply_glyph_mask_lane` contract; this packet adds a later run075 caller
boundary and does not claim that the CPU addresses are native page offsets.

Authority: `build/run075_frame540_c330fe_trace/trace.jsonl`, captured from
`captures/run075/restored-state.bin` and `captures/run075/playback.e9k` with a
breakpoint at `$C330FE`, armed at frame 540. The trace reached the breakpoint
at frame 584 and was bounded to 1,200 instructions.
