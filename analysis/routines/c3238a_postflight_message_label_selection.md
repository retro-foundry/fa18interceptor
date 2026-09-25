# `$C3238A-$C32463`: postflight label/template selection

Classification: **static dataflow in a runtime-observed postflight parent path**.

`source_amiga/observed/select_postflight_message_labels.asm` is byte exact for
`$C3238A-$C32463`. It copies an all-space literal from `$C326B8` into
`$C4580A`, then chooses a NUL-terminated secondary literal for `$C4580E` from
the signed record at `$C46184 + D2`.

The immutable literal table directly establishes these labels: `NO SIG`,
`M1G-29`, `M1G-25`, `AF-1`, `F-16`, `CRUISE`, `707`, and `767`; the adjacent
`ALT:`, `HDG:`, and `SPD:` strings are table data but are not selected by this
specific block. Record bit 6 at `+$20` selects `NO SIG` when clear. Otherwise
the byte at `+$62` selects the other labels for observed codes `$10`,
`$12-$17`, while changing only bits 6/7 of `$C45862`.

The code-to-label mapping and bounded string-copy behavior are proven. It does
not prove the record class, UI field ownership, target identity, or a
qualification/mission outcome.
