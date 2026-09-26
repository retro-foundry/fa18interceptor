; Byte-exact observed postflight callback prefix $C10E96-$C10E9F.
; It loads the shared signed callback-state byte and takes the ordinary
; callback-code path when that byte is nonnegative.

                org     $C10E96

POSTFLIGHT_CALLBACK_SIGNED_STATE equ     $C457AE

gate_postflight_callback_signed_state:
                move.b  POSTFLIGHT_CALLBACK_SIGNED_STATE.l,d0
                tst.b   d0
                bpl.b   $C10ECE
