; Byte-exact observed matrix-product magnitude classifier $C2E016-$C2E02B.
; It negates the signed shared product, derives its absolute value in D1, then
; selects the low-magnitude continuation when it is below $0E210000.

                org     $C2E016

MATRIX_PRODUCT_SHARED_VALUE     equ     $C45BBE

classify_matrix_product_magnitude:
                move.l  MATRIX_PRODUCT_SHARED_VALUE,d0
                neg.l   d0
                move.l  d0,d1
                bge.b   $C2E024
                neg.l   d1
                cmpi.l  #$0E210000,d1
                blt.b   $C2E07C
