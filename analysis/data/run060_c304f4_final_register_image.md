# run060 `$C304F4` final register image

The run060 replay was stopped at the first instruction of `$C304F4`, the
`BLTSIZE` write. The custom-register image captured at that exact boundary is:

| Register | Value |
| --- | ---: |
| `BLTCON0` | `$00FC` |
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
`BLTSIZE`. The image therefore differs from the `$C304B2` setup packet. The
`$00FC` control readback is retained as evidence but is not yet assigned a
semantic minterm: the preceding caller path must explain the control change
before this register image can drive the native renderer.

Authority: `build/run060_c304f4_final_regs/custom_at_breakpoint.bin` and
`trace.jsonl`. This capture uses run060 only.
