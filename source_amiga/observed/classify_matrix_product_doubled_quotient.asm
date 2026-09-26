; Byte-exact observed matrix-product continuation $C2E116-$C2E121.
; It restores the quotient word, doubles it, then routes negative or at-most
; $180 values to their respective table-lookup continuations.

                org     $C2E116

classify_matrix_product_doubled_quotient:
                swap    d0
                add.w   d0,d0
                blt.b   $C2E174
                cmpi.w  #$180,d0
                ble.b   $C2E16E
