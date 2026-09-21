; Byte-exact observed signed-D2 guard $C2DEEC-$C2DEEF.

                org     $C2DEEC

CONTINUE_NONNEGATIVE_D2         equ     $C2DEF4

guard_signed_matrix_product_d2:
                tst.w   d2
                bge.b   CONTINUE_NONNEGATIVE_D2
