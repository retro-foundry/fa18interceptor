# run060 frame 7992 `$C30668` preparation trace

A breakpoint at `$C30668` was replayed from
`captures/run060/restored-state.bin` through frame 7992 using the run060
deterministic input stream. It stopped 36 times, matching the 36 ordered
`$DFF058` submissions in the DMA inventory. The live capture is retained in
`build/run060_blit_entries7992.json`.

## Repeated preparation sequence

The calls occur as four plane passes of the same nine preparation shapes. The
first pass has calls 0--8, the second 9--17, the third 18--26, and the fourth
27--35.

| shape | `D0` | `D1` | `A1` | `A4` | representative `BLTCON0/1` |
|---:|---:|---:|---:|---:|---|
| 0 | 319 | 67 | 1 | 88 | `$0100/$0000` |
| 1 | 0 | 67 | 1 | 88 | `$FB4A/$0043` |
| 2 | 319 | 83 | 94 | 111 | `$FB0A/$0051` |
| 3 | 18 | 87 | 94 | 145 | `$0B4A/$0053` |
| 4 | 0 | 67 | 116 | 28 | `$1B4A/$0057` |
| 5 | 319 | 67 | 101 | 43 | `$0B4A/$0043` |
| 6 | 222 | 67 | 163 | 9 | `$0722/$0000` |
| 7 | 24 | 67 | 163 | 9 | `$EB4A/$0043` |
| 8 | 319 | 67 | 1 | 88 | `$0100/$0000` |

`$C30668` is a preparation boundary. The DMA inventory remains authoritative
for the final register writes and completed hardware submissions.

## Port implication

The native renderer should add a semantic prepared packet containing the
screen endpoints or spans, line control word, masks, and plane selection. The
four plane passes can then submit the same packet family to the semantic page.
The observed `D0/D1` values are not yet promoted to screen endpoints; their
relationship to `A1/A4` and the source table needs one bounded caller trace.
Frame 7992 remains held until all 36 outputs are reproduced in submission
order and the final chunky page matches the emulator.
