; Byte-exact reconstruction of $C2E5AC-$C2E5F5 (Hunk 32 +$1A4).
; Input: A1 points to nine signed words (three consecutive matrix rows).
; Effect: scales each row by its corresponding word in $C45A3E, using >> 8.
; A1 advances by 12 bytes.

                org     $C2E5AC

MATRIX_ROW_SCALE_VECTOR equ $C45A3E
FIXED_PRODUCT_SHIFT     equ 8

scale_matrix_rows:
                movem.w MATRIX_ROW_SCALE_VECTOR,d0-d2

                movem.w (a1),d3-d5
                muls.w  d0,d3
                muls.w  d0,d4
                muls.w  d0,d5
                asr.l   #FIXED_PRODUCT_SHIFT,d3
                asr.l   #FIXED_PRODUCT_SHIFT,d4
                asr.l   #FIXED_PRODUCT_SHIFT,d5
                movem.w d3-d5,(a1)
                addq.w  #6,a1

                movem.w (a1),d3-d5
                muls.w  d1,d3
                muls.w  d1,d4
                muls.w  d1,d5
                asr.l   #FIXED_PRODUCT_SHIFT,d3
                asr.l   #FIXED_PRODUCT_SHIFT,d4
                asr.l   #FIXED_PRODUCT_SHIFT,d5
                movem.w d3-d5,(a1)
                addq.w  #6,a1

                movem.w (a1),d3-d5
                muls.w  d2,d3
                muls.w  d2,d4
                muls.w  d2,d5
                asr.l   #FIXED_PRODUCT_SHIFT,d3
                asr.l   #FIXED_PRODUCT_SHIFT,d4
                asr.l   #FIXED_PRODUCT_SHIFT,d5
                movem.w d3-d5,(a1)
                rts
