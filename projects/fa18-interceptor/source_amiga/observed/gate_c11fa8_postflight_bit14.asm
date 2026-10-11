; Byte-exact observed basic block $C11FA8-$C11FB7.
; The bit-set route is not exercised by the available captures.

                org     $C11FA8

POSTFLIGHT_STATE_WORD           equ     $C45AE0
POSTFLIGHT_STATE_COPY_PATH      equ     $C1205C

gate_c11fa8_postflight_bit14:
                move.w  POSTFLIGHT_STATE_WORD.l,d0
                btst    #14,d0
                beq.w   POSTFLIGHT_STATE_COPY_PATH
