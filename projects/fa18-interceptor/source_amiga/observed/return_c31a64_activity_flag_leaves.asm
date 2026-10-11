; Byte-exact observed C31A64 activity-flag boundary $C31AC8-$C31AD3.
; Two preceding leaves return; the following signed shared-byte test loops to
; the second leaf while the value is nonpositive.

                org     $C31AC8

POSTFLIGHT_ACTIVITY_FLAG        equ     $C4583D

                rts
                rts
                tst.b   POSTFLIGHT_ACTIVITY_FLAG.l
                ble.b   $C31ACA
