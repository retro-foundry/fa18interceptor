# run060 `$C304F4` final register image

The run060 replay was stopped at the first instruction of `$C304F4`, the
`BLTSIZE` write. The custom-register image captured at that exact boundary is:

| Register | Value |
| --- | ---: |
| `BLTCON0` readback | `$00FC` |
| `BLTCON1` | `$0002` |
| `BLTAFWM/BLTALWM` | `$00FF/$00FF` |
| `BLTCPT` | `$00000037` |
| `BLTBPT` | `$00010066` |
| `BLTAPT` | `$000000EE` |
| `BLTDPT` | `$00010066` |
| `BLTCMOD/BLTBMOD/BLTAMOD/BLTDMOD` | `$0028/$0001/$0001/$0001` |
| `BLTBDAT/BLTADAT` | `$00FF/$0000` |
| `BLTSIZE` before the instruction | `$0014` |

The CPU register at the trigger is `D0=$0D14`; `$C304F4` writes that value to
`BLTSIZE`. The return bounded trace shows the preceding branch writes
`move.w #$0DFC,$40(a0)` before the common `$C304B2` setup. The custom readback
is `$00FC`, so the high control bits are not preserved by the readback image;
the programmed final control word is `$0DFC`. This is the actual final mode
for the run060 fill submission.

Authority: `build/run060_c304f4_final_regs/custom_at_breakpoint.bin` and
`trace.jsonl`. This capture uses run060 only.
