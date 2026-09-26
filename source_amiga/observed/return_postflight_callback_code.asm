; Byte-exact postflight callback tail $C11040-$C1104B.
; It publishes the observed callback code $0F, releases the helper frame, and
; returns.

                org     $C11040

POSTFLIGHT_CALLBACK_CODE        equ     $C458A1

return_postflight_callback_code:
                move.b  #$f,POSTFLIGHT_CALLBACK_CODE.l
                unlk    a6
                rts
