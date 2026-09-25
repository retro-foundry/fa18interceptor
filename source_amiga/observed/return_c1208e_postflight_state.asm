; Byte-exact observed exit block $C1208E-$C12097.

                org     $C1208E

POSTFLIGHT_AUXILIARY_FLAG       equ     $C4588E

return_c1208e_postflight_state:
                clr.b   POSTFLIGHT_AUXILIARY_FLAG.l
                unlk    a6
                rts
