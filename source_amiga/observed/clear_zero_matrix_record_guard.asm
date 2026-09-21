; Byte-exact matrix-path guard entry $C2DE96-$C2DEA1.

                org     $C2DE96

MATRIX_RECORD_WORD_50           equ $50
MATRIX_GUARD_CONTINUATION       equ $C2DEB0

clear_zero_matrix_record_guard:
                tst.w   MATRIX_RECORD_WORD_50(a1)
                bne.b   MATRIX_GUARD_CONTINUATION
                clr.w   MATRIX_RECORD_WORD_50(a1)
                rts
