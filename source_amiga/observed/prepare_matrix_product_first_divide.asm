; Byte-exact observed matrix-product continuation $C2E0DC-$C2E0E9.
; It negates the first shared long numerator and selects the direct divide
; route when D3 does not exceed the observed $147 threshold.

                org     $C2E0DC

MATRIX_PRODUCT_NUMERATOR_A     equ     $C45BBA

prepare_matrix_product_first_divide:
                move.l  MATRIX_PRODUCT_NUMERATOR_A.l,d0
                neg.l   d0
                cmpi.w  #$147,d3
                bgt.b   $C2E0F0
