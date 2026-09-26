# run060 frame 7991 renderer profile

The Engine9000 profiler was started for replay frames 7988 through 7991 after
restoring `captures/run060/restored-state.bin`. The profile captured 249,001
cycles across 4,096 PCs. The relevant renderer addresses were all executed in
the frame window:

| routine/address | profiler cycles |
|---|---:|
| `$C2FEDE` selected table display stage | 14 |
| `$C2FF48` polygon submission wrapper | 8 |
| `$C301F6` area submission continuation | 44 |
| `$C30466` lane control path | 64 |
| `$C304F4` final blitter setup entry | 38 |
| `$C304F8` blitter wait/submit continuation | 22,911 |
| `$C305D6` unequal pair continuation | 6 |
| `$C30668` prepared blitter submission | 12 |
| `$C306AE` active plane continuation | 36 |

The high `$C304F8` count is the blitter idle wait loop. This confirms that the
frame 7991 fill is reached through the normal selected-table and polygon
submission path, then spends most of its time waiting for the hardware
operation to finish. The native port should model that as a synchronous
semantic operation at the `$C30668` submission boundary; it should not model
the wait loop as emulated CPU time.

This profile identifies the call chain but does not establish the complete
source and destination page mapping. The settled pixel spans and live register
sequence remain the output authority for the current frame.
