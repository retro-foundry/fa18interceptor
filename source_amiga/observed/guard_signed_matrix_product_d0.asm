; Byte-exact observed signed-D0 guard $C2DEE0-$C2DEE7.

                org     $C2DEE0

CONTINUE_NONNEGATIVE_D0         equ     $C2DEEC

guard_signed_matrix_product_d0:
                movem.l d1/a0-a1,-(a7)
                tst.w   d0
                bge.b   CONTINUE_NONNEGATIVE_D0
