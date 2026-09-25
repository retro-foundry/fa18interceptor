; Byte-exact nonzero qualification-mode guard $C31F72-$C31F85.
; Field ownership and scenario role remain unresolved.

                org     $C31F72

NONZERO_MODE_GUARD_BYTE         equ     $C457D9
NONZERO_MODE_GUARD_FLAGS        equ     $C458CD
RETURN_QUALIFICATION_LINE       equ     $C31F92
PREPARE_QUALIFICATION_LINE      equ     $C31F94

guard_nonzero_qualification_mode:
                tst.b   NONZERO_MODE_GUARD_BYTE.l
                beq.b   RETURN_QUALIFICATION_LINE
                btst.b  #6,NONZERO_MODE_GUARD_FLAGS.l
                beq.b   RETURN_QUALIFICATION_LINE
                bra.b   PREPARE_QUALIFICATION_LINE
