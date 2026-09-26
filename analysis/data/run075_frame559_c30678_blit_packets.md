# run075 frame 559 `$C30678` blit packets

The full `$C2FF48` trace reaches the `$C30678` blitter setup three times after
the pair consumer has processed the transition records. The entry register
packets are:

| Pass | `D0` | `D1` | `D2` | `D3` | `D4` | `D5` | `D6` | `D7` |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | `$FFFFFFFF` | `$0053` | `$FF16` | `$0004` | `$1E02` | `$FDC8` | `$8AEA` | `$6E71` |
| 2 | `$FFFFFFFF` | `$0043` | `$FF52` | `$0000` | `$0D42` | `$FEA4` | `$FACE` | `$6EEF` |
| 3 | `$FFFFFFFF` | `$0043` | `$FF4E` | `$0000` | `$0DC2` | `$FEBc` | `$0B4A` | `$6E58` |

The setup writes the corresponding values into the semantic blit operation,
including the fixed `$28` width and height words observed in the trace. The
three packets are the display side of the frame559 polygon path and are the
next inputs for the native blit implementation.

Authority: `build/run075_c2ff48_559_full/trace.jsonl`, entries at the three
branches to `$C30678`, captured from the canonical run075 restore and
playback.
