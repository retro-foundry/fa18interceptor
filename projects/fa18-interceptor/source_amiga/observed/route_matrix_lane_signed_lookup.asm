; Byte-exact observed matrix-lane signed lookup routes $C2E16E-$C2E187.
; One route reads the signed lookup word directly; the adjacent route negates
; the index and selects either a direct lookup continuation or quotient path.

                org     $C2E16E

route_matrix_lane_signed_lookup:
                move.w  $0(a1,d0.w),d2
                bra.b   $C2E1D4

negate_matrix_lane_index:
                neg.w   d0
                cmpi.w  #$180,d0
                ble.b   $C2E1CA
                move.l  $C45BC2,d0
                cmpi.w  #$147,d3
                bgt.b   $C2E18E
