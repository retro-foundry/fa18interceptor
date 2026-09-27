# run060 frame 7999 DMA sequence

Source: `build/run060_dma_frame7999_wide.json`, captured from the deterministic run060 replay. The frame contains ten `$DFF058` submissions.

| Job | Observed operation | Evidence | Native status |
|---:|---|---|---|
| 0 | Four plane setup and packed word activity | `$DFF040=$EB4A`, `$DFF042=$43`; 42 writes in each of four type-1/2/3 streams | Open |
| 1 | Line packet | `$DFF040=$1B4A`, `$DFF042=$43`; 10 C words and 10 D words at a 40-byte row stride | Covered by `run060_frame7999_line_packets.h` |
| 2 | Word clear or copy stream | `$DFF040=$09F0`, `$DFF042=$0A`; ten C/D pairs at a 40-byte row stride | Open; line mode is disabled by BLTCON1 bit 0 |
| 3 | Descending two source streams | `$DFF040=$0FEC`, `$DFF042=$02`; 22 writes in each type-16/19 stream | Open |
| 4 | Four plane descending stream | `$DFF040=$0D0C`, `$DFF042=$02`; 22 writes in each plane stream | Open |
| 5 | Four plane setup followed by line submission | `$DFF040=$CB0A`, `$DFF042=$51`; type-0/1/3 writes precede the line packet | Open for the setup stream; line packet is job 7 |
| 6 | Line packet with repeated destination words | `$DFF040=$CBFA`, `$DFF042=$51`; 11 C/D rows, first four writes repeat one word | Covered by `run060_frame7999_line_packets.h` |
| 7 | Line packet with repeated destination words | `$DFF040=$CB0A`, `$DFF042=$51`; 11 C/D rows, first four writes repeat one word | Covered by `run060_frame7999_line_packets.h` |
| 8 | Display page line packet plus CPU and display DMA | `$DFF040=$030A`, `$DFF042=$00`; 11 C/D rows and extensive type-6 page reads | The C/D packet is captured; page operation remains open |
| 9 | Copper/display list activity | `$DFF09A` alternates display values while `$DFF004/$DFF006` update beam positions | Open |

Jobs 1, 6, and 7 use the line executor's semantic packet boundary. Jobs 2 and 8 have BLTCON1 bit 0 clear and must be modeled through the area or word stream path. Jobs 0, 3, 4, 5, and 9 are the active frame gate.
