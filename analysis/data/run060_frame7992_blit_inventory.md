# run060 frame 7992 ordered blit inventory

`scripts/inventory_dma_blits.py` reconstructs each `$DFF058` submission from
the widened DMA capture. Frame 7992 contains 36 ordered jobs:

| jobs | `BLTCON0` | `BLTCON1` | `BLTSIZE` | destination planes |
|---:|---:|---:|---:|---|
| 0--3 | `$0FCE` | `$0000` | `$0312` | 3, 2, 1, 0 |
| 4--7 | `$DB0A/$DBFA` | `$0051` | `$0142` | 0, 1, 2, 3 |
| 8--11 | `$DB0A/$DBFA` | `$0051` | `$0142` | 0, 1, 2, 3 |
| 12--15 | `$DB0A/$DBFA` | `$0051` | `$0142` | 0, 1, 2, 3 |
| 16--19 | `$5B0A` | `$0041` | `$18C2` | 0, 1, 2, 3 |
| 20--23 | `$DB0A/$DBFA` | `$0015` | `$0242` | 0, 1, 2, 3 |
| 24--27 | `$DB0A/$DBFA` | `$0051` | `$2142` | 0, 1, 2, 3 |
| 28--31 | `$1B0A/$1BFA` | `$0011` | `$0242` | 0, 1, 2, 3 |
| 32--35 | `$9B0A` | `$0041` | `$18C2` | 0, 1, 2, 3 |

All later jobs use `$28` C/D modulos and fan out to the four semantic planes.
Their destination offsets and source asset selections still need to be
resolved before frame 7992 can use the complete native sequence.
