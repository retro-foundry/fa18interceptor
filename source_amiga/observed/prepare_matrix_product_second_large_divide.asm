; Byte-exact second matrix-product large-divide setup $C2E18E-$C2E193.

                org     $C2E18E

prepare_matrix_product_second_large_divide:
                asr.l   #6,d0
                move.w  d3,d2
                bge.b   $C2E196
