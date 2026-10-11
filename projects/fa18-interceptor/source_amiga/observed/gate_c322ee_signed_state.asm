; Byte-exact observed signed-state gate $C322EE-$C322F7.
; The nonnegative continuation is outside the captured slice.

                org     $C322EE

SIGNED_STATE_C459C0             equ     $C459C0
SIGNED_STATE_NEGATIVE            equ     $C32510

gate_c322ee_signed_state:
                move.w  SIGNED_STATE_C459C0.l,d2
                blt.w   SIGNED_STATE_NEGATIVE
