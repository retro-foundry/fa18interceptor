; Byte-exact matrix-product second-lane lookup $C2E29A-$C2E29F.

                org     $C2E29A

load_matrix_product_second_lane_lookup:
                move.w  (a1,d0.w),d6
                bra.b   $C2E300
