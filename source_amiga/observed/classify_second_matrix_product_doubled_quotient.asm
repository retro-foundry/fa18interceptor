; Byte-exact observed second matrix-product continuation $C2E242-$C2E24D.
; It restores and doubles the alternate quotient word, then routes negative
; and at-most-$180 values to their alternate table-lookup paths.

                org     $C2E242

classify_second_matrix_product_doubled_quotient:
                swap    d0
                add.w   d0,d0
                blt.b   $C2E2A0
                cmpi.w  #$180,d0
                ble.b   $C2E29A
