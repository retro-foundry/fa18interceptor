; Byte-exact observed basic block $C11C44-$C11C4F.
; The taken branch body at $C11C50 is intentionally not reconstructed here.

                org     $C11C44

POSTFLIGHT_FLAG_WORD            equ     $C458D6
POSTFLIGHT_CONTEXT_TEST         equ     $C11C5A

gate_c11c44_postflight_flag:
                move.w  POSTFLIGHT_FLAG_WORD.l,d0
                btst    #5,d0
                beq.b   POSTFLIGHT_CONTEXT_TEST
