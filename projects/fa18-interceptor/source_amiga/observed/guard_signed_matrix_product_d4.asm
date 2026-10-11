; Byte-exact observed signed-D4 guard $C2DEF4-$C2DEF7.

                org     $C2DEF4

CONTINUE_MATRIX_PRODUCT         equ     $C2DEFC

guard_signed_matrix_product_d4:
                tst.w   d4
                bge.b   CONTINUE_MATRIX_PRODUCT
