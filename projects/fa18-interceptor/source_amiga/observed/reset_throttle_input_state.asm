; Byte-exact helper $C1B602-$C1B615, called by throttle-mode selection.

                org     $C1B602

THROTTLE_RESET_BYTE            equ $C45870
CONTROL_ACCUMULATOR_Y           equ $C45778
CONTROL_ACCUMULATOR_COMPANION   equ $C4577C

reset_throttle_input_state:
                clr.b   THROTTLE_RESET_BYTE.l
                clr.w   CONTROL_ACCUMULATOR_Y.l
                clr.w   CONTROL_ACCUMULATOR_COMPANION.l
                rts
