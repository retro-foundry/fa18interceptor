; Byte-exact static-only scaled-helper setup $C334DE-$C334F9.
                org     $C334DE
POSTFLIGHT_HELPER equ $C33F8A
POSTFLIGHT_STATUS_LONG equ $C45B26
invoke_postflight_scaled_helper:
                addi.l  #$E02,d5
                move.l  d5,-(a7)
                move.w  d1,-(a7)
                bsr.w   POSTFLIGHT_HELPER
                move.l  #5,POSTFLIGHT_STATUS_LONG.l
                move.w  (a7)+,d1
                move.l  (a7)+,d5
