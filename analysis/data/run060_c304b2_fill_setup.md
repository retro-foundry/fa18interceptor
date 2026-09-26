# run060 `$C304B2` area fill setup

The run060 replay was traced at `$C304B2`, immediately before its return to
the `$C304F4` trigger. The first invocation after the frame-7991 fill probe
programs the area blitter with:

| Register | Value | Evidence |
| --- | ---: | --- |
| `BLTCON0` | `$0D0C` | `move.w #$d0c,$40(a0)` |
| `BLTCON1` | `$0002` | `move.w #$2,$42(a0)` |
| `BLTDPTH` | `$000076EE` | `move.l d2,$50(a0)` |
| `BLTCPTH` | `$000076EE` | `move.l d1,$4C(a0)` |
| `BLTDPTL` | `$000076EE` | `move.l d1,$54(a0)` |
| `BLTSIZE` | `$0D14` | `move.w d0,$58(a0)` |

The caller image at entry also contains `BLTAFWM=$00FF`, `BLTALWM=$00FF`,
`BLTCMOD=$0028`, `BLTBMOD=$0001`, `BLTAMOD=$0001`, and `BLTDMOD=$0000`.
The pointer image before the helper writes is `C=$00010026`, `B=$00000037`,
`A=$000000EE`, and `D=$00010026`; the helper replaces A, B, and D with the
`$000076EE` values above and leaves C unchanged.

The entry values are `D0=$0D14`, `D1=$000076EE`, and `D2=$000076EE`.
`BLTCON1` bit 1 selects descending address progression; bit zero is clear,
so this is an area fill operation rather than line mode. The setup writes the
C and D channels to the same destination word address, while the A and B
channels are not written by this helper invocation. The remaining A/B/C/D
modulos and first/last word masks must be recovered from the preceding
caller state before this operation can be implemented as a native fill.

Authority: `build/run060_c304b2_fill_setup/trace.jsonl`, beginning at trace
index 3 and ending at index 13. This capture uses run060 and stays within the
project policy of using runs 60 and above.
