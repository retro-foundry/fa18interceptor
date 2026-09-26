# Run060 `$C1822A` graphics ViewPort structure

Classification: **runtime layout plus OS-API contract**.

`$C1612C` passes the literal address `$C1822A` in `A0` to `$C53F88`. The
wrapper uses the live `graphics.library` base at `$C182CA` and its `-$192`
LVO, which the pinned Kickstart 1.3 headers identify as
`WaitBOVP(struct ViewPort *vp)`. Thus `$C1822A` is a live graphics ViewPort
argument, not an untyped callback buffer.

The run060 frame-9,284 Slow-RAM checkpoint independently has this prefix at
`$C1822A`:

| ViewPort offset | Live value | Structural interpretation |
| ---: | ---: | --- |
| `+$00` | `$00000000` | null `Next` pointer |
| `+$04` | `$00C074D0` | pointer-sized ColorMap-position field |
| `+$08` | `$00C01488` | pointer-sized display-instruction field |
| `+$0C-$14` | zero | adjacent pointer fields |
| `+$18/+1A` | `$0140/$00C8` | display dimensions 320 × 200 |

This prefix is consistent with the Kickstart `struct ViewPort` layout and
cross-checks the `WaitBOVP` call signature. The joystick/control callback at
`$C1718E` copies 16 longwords of selected mode state into this same structure;
that proves a mode-state producer for the ViewPort bytes, not a complete
display-mode or camera interpretation.

The name applies only to `$C1822A` as the graphics API argument. Nearby
pointer fields and the exact ownership of the copied templates remain
unassigned.
