# run060 frame 7991 DMA blitter sequence

The rebuilt Engine9000 core was run from `captures/run060/restored-state.bin`
with `captures/run060/playback.e9k` applied through replay frame 7991. DMA
collection mode 6 reported frame 7991 with 288,000 raw records and a 720 by
287 render surface. The capture tool is `scripts/capture_dma_frame.py`.

The records around the visible fill call are at vertical position 84. They
show the 68k writing the `$C304B2` helper and the custom blitter registers:

| hpos | bus address | value |
|---:|---:|---:|
| 0 | `$C304D6` | `$0D0C` |
| 2 | `$C304D8` | `$0040` |
| 6 | `$DFF040` BLTCON0 | `$0D0C` |
| 8 | `$C304DC` | `$317C` |
| 10 | `$C304E2` | `$317C` |
| 13 | `$C304E4` | `$0002` |
| 19 | `$DFF042` BLTCON1 | `$0002` |
| 27 | `$DFF052` BLTAPTL | `$76EE` |
| 33 | `$DFF04C` BLTBPTH | `$0000` |
| 35 | `$DFF04E` BLTBPTL | `$76EE` |
| 41 | `$DFF054` BLTCPTH | `$0000` |
| 43 | `$DFF056` BLTCPTL | `$76EE` |
| 49 | `$DFF058` BLTDPTH | `$0D14` |

The pointer and size writes continue after this sequence. The `$C304B2`
entry is therefore confirmed as live frame construction evidence, rather than
an isolated function trace. The raw capture also includes the subsequent
polygon memory accesses beginning at hpos 53; those accesses should be used
to identify the source and destination page before connecting the semantic
fill executor to the native renderer.

The capture is evidence for the run060 frame only. It does not authorize
advancing the port to a later frame until the native output for frame 7991 is
matched.
