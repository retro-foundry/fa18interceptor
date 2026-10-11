; Byte-exact observed 3x3 signed matrix-product block $C2DEFC-$C2E017.

                org     $C2DEFC

MATRIX_PRODUCT_LEFT             equ     $C45B90
MATRIX_PRODUCT_RESULT           equ     $C45BA2
PREPARE_MATRIX_PRODUCT_INPUTS   equ     $C2E47A

compute_signed_matrix_product:
                lea.l   MATRIX_PRODUCT_LEFT.l,a1
                bsr.w   PREPARE_MATRIX_PRODUCT_INPUTS
                lea.l   (a4),a1
                lea.l   MATRIX_PRODUCT_LEFT.l,a2
                lea.l   MATRIX_PRODUCT_RESULT.l,a3

; row 0 · columns 0, 1, 2
                move.w  (a1),d0
                muls.w  (a2),d0
                move.w  6(a1),d1
                muls.w  2(a2),d1
                move.w  12(a1),d2
                muls.w  4(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  2(a1),d0
                muls.w  (a2),d0
                move.w  8(a1),d1
                muls.w  2(a2),d1
                move.w  14(a1),d2
                muls.w  4(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  4(a1),d0
                muls.w  (a2),d0
                move.w  10(a1),d1
                muls.w  2(a2),d1
                move.w  16(a1),d2
                muls.w  4(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+

; row 1 · columns 0, 1, 2
                move.w  (a1),d0
                muls.w  6(a2),d0
                move.w  6(a1),d1
                muls.w  8(a2),d1
                move.w  12(a1),d2
                muls.w  10(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  2(a1),d0
                muls.w  6(a2),d0
                move.w  8(a1),d1
                muls.w  8(a2),d1
                move.w  14(a1),d2
                muls.w  10(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  4(a1),d0
                muls.w  6(a2),d0
                move.w  10(a1),d1
                muls.w  8(a2),d1
                move.w  16(a1),d2
                muls.w  10(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+

; row 2 · columns 0, 1, 2
                move.w  (a1),d0
                muls.w  12(a2),d0
                move.w  6(a1),d1
                muls.w  14(a2),d1
                move.w  12(a1),d2
                muls.w  16(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  2(a1),d0
                muls.w  12(a2),d0
                move.w  8(a1),d1
                muls.w  14(a2),d1
                move.w  14(a1),d2
                muls.w  16(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)+
                move.w  4(a1),d0
                muls.w  12(a2),d0
                move.w  10(a1),d1
                muls.w  14(a2),d1
                move.w  16(a1),d2
                muls.w  16(a2),d2
                add.l   d1,d0
                add.l   d2,d0
                move.l  d0,(a3)
