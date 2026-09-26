; Byte-exact observed postflight callback continuation $C10FC8-$C10FD3.
; It tests the shared callback-state byte for the $FF sentinel before the
; following postflight route.

                org     $C10FC8

POSTFLIGHT_CALLBACK_STATE       equ     $C4582A

gate_postflight_callback_state_sentinel:
                move.b  POSTFLIGHT_CALLBACK_STATE.l,d0
                cmpi.b  #$ff,d0
                bne.b   $C10FEE
