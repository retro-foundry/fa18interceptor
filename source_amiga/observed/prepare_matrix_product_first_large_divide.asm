; Byte-exact first matrix-product large-divide setup $C2E0F0-$C2E0F5.

                org     $C2E0F0

prepare_matrix_product_first_large_divide:
                asr.l   #6,d0
                move.w  d3,d2
                bge.b   $C2E0F8
