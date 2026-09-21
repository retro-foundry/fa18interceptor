; Byte-exact static-only flagged transform wrapper $C33CC4-$C33CD1.
                org     $C33CC4
POSTFLIGHT_FLAG_BYTE            equ $C457AE
POSTFLIGHT_TRANSFORM            equ $C33CD2
run_postflight_flagged_transform:
                tst.b   POSTFLIGHT_FLAG_BYTE.l
                bne.s   postflight_flagged_transform_done
                bsr.w   POSTFLIGHT_TRANSFORM
postflight_flagged_transform_done:
                rts
