; Byte-exact reconstruction of $C2DAE0-$C2DAF1 (Hunk 32 +$6D8).
; Effect: copies three words of matrix-adjacent state to the active cache.

                org     $C2DAE0

MATRIX_AUXILIARY_SOURCE equ $C461EA
MATRIX_AUXILIARY_CACHE  equ $C45A88

copy_matrix_auxiliary_words:
                movem.w MATRIX_AUXILIARY_SOURCE,d0-d2
                movem.l d0-d2,MATRIX_AUXILIARY_CACHE
                rts
