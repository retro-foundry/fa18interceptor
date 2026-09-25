; Byte-exact observed basic block $C11E3C-$C11E4B.

                org     $C11E3C

POSTFLIGHT_STATE_WORD           equ     $C45AE0
POSTFLIGHT_HIGH_BIT_CLEAR_PATH  equ     $C11FA8

gate_c11e3c_postflight_high_bit:
                move.w  POSTFLIGHT_STATE_WORD.l,d0
                andi.w  #$8000,d0
                tst.w   d0
                beq.w   POSTFLIGHT_HIGH_BIT_CLEAR_PATH
