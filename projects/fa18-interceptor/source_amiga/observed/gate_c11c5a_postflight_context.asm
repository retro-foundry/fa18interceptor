; Byte-exact observed basic block $C11C5A-$C11C69.
; The following context-present body begins at $C11C6A and is outside this
; observed gate slice.

                org     $C11C5A

POSTFLIGHT_CONTEXT_POINTER      equ     $C45B50
POSTFLIGHT_CONTEXT_VALID        equ     $C4586E
POSTFLIGHT_CLEAR_PATH           equ     $C11D5A

gate_c11c5a_postflight_context:
                tst.l   -$c(a6)
                bne.b   $C11C6A
                tst.b   POSTFLIGHT_CONTEXT_VALID.l
                beq.w   POSTFLIGHT_CLEAR_PATH
