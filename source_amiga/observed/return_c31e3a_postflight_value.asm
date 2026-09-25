; Byte-exact observed common postflight completion $C31E3A-$C31E5D.

                org     $C31E3A

POSTFLIGHT_FORMAT_VALUE         equ     $C25A08
POSTFLIGHT_SUBMIT_VALUE         equ     $C32AA4
POSTFLIGHT_FORMAT_ARGUMENT      equ     $C45B1E

return_c31e3a_postflight_value:
                lea     $3(a2),a0
                moveq   #2,d6
                moveq   #1,d7
                ext.l   d0
                move.l  d0,POSTFLIGHT_FORMAT_ARGUMENT.l
                jsr     POSTFLIGHT_FORMAT_VALUE.l
                move.w  d2,d0
                move.w  d7,d2
                swap    d0
                move.w  d6,d0
                bsr.w   POSTFLIGHT_SUBMIT_VALUE
                rts
