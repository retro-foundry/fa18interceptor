; Byte-exact observed matrix-product continuation $C2E208-$C2E215.
; It negates the alternate shared long numerator and selects its direct divide
; route when D3 does not exceed the observed $147 threshold.

                org     $C2E208

MATRIX_PRODUCT_NUMERATOR_B     equ     $C45BA6

prepare_matrix_product_second_divide:
                move.l  MATRIX_PRODUCT_NUMERATOR_B.l,d0
                neg.l   d0
                cmpi.w  #$147,d3
                bgt.b   $C2E21C
