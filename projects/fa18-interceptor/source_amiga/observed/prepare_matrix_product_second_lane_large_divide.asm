; Byte-exact second-lane matrix-product large-divide setup $C2E21C-$C2E221.

                org     $C2E21C

prepare_matrix_product_second_lane_large_divide:
                asr.l   #6,d0
                move.w  d3,d6
                bge.b   $C2E224
