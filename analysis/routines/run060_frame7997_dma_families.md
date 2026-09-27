# run060 frame 7997 DMA families

The sealed run060 capture contains 60 blitter submissions at frame 7997.
Grouping the submissions by the captured control and extent registers reduces
the work to reusable packet families. The first family appears at jobs 0 and 2:

| field | value |
|---|---:|
| BLTCON0 | `$FFFFEBFA` |
| BLTCON1 | `$55` |
| BLTAMOD | `$FFFFFFE8` |
| BLTBMOD | `$0000` |
| BLTCMOD | `$0005` |
| BLTDMOD | `$0005` |
| BLTSIZE | `$0342` |

Each submission has 13 captured line C words and 13 D writes. The two packets
share the control and recurrence state; their destination pointers select two
display destinations. The other plane submissions use related `BLTCON0`
values, preserving this family shape while changing the line direction,
extent, or destination plane.

The DMA records for these two instances repeatedly report the same destination
word during the 13 line iterations. The C/D event stream therefore represents
pipeline writes to one final word, rather than thirteen adjacent words. The
native oracle must compare the final word at each destination after executing
the recurrence.

## Port contract

Represent this as a semantic line packet containing the line recurrence fields,
the captured C source stream, and a destination plane/offset. Keep the two
instances separate at the caller level so destination selection remains an
explicit game state decision. Do not model the original absolute Chip RAM
addresses as a native memory image.
