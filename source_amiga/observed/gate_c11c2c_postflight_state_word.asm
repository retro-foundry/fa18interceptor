; Byte-exact observed basic block $C11C2C-$C11C3B.
; The alternate branch body at $C11C3C is not reconstructed in this slice.

                org     $C11C2C

POSTFLIGHT_STATUS_WORD          equ     $C458C8
POSTFLIGHT_NEXT_STATE_TEST      equ     $C11C44

gate_c11c2c_postflight_state_word:
                move.w  POSTFLIGHT_STATUS_WORD.l,d0
                andi.w  #$8000,d0
                tst.w   d0
                beq.b   POSTFLIGHT_NEXT_STATE_TEST
