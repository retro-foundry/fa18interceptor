# Run031 frame-12,000 polygon projection sample

Classification: **runtime-backed projection-to-renderer sample**.

A no-input replay restored from the frame-12,000 Golden Gate checkpoint hit
`$C2FF48` at replay frame 1. At that breakpoint `$C4B390` contains a
four-point screen-pair list produced by `$C24CFE` from the first four triples
at `$C4B990`.

| `$C4B990` triple `(x, y, z)` | Calculated screen `(x, y)` | Stored `$C4B390` pair |
| --- | --- | --- |
| `(-799, 793, 2444)` | `(108, 119)` | `(211, 60)` |
| `(-866, 786, 2421)` | `(103, 119)` | `(216, 60)` |
| `(-1474, 882, 3455)` | `(92, 112)` | `(227, 67)` |
| `(-1407, 889, 3478)` | `(96, 113)` | `(223, 66)` |

The stored pair is the exact reflected form `(319 - screen_x, 179 -
screen_y)`. `$C4B390` begins with count `$0004`; the renderer trace then
enters `$C2FF48`, clears the blitter interrupt state, and passes this list to
`$C301F6`.

## Evidence

`build/run031_frame12000_c2ff48_noinput_probe/snapshot.json` records the
`$C2FF48` breakpoint. Its `slow.bin` contains the captured lists, and
`trace.jsonl` starts with the `$C2FF48 -> $C301F6` submission transition.

This proves a concrete record-to-projected-pair handoff. It is not yet a
pixel-to-landmark attribution: checkpoint screenshots are not a valid visual
oracle, and the particular source control entry has not been isolated.
