; Byte-exact C332B4-C332BB state clear immediately after the C33258 table.
                org     $C332B4
POSTFLIGHT_AUXILIARY_FLAG       equ     $C458B4

clear_postflight_auxiliary_flag:
                clr.b   POSTFLIGHT_AUXILIARY_FLAG.l
                rts
