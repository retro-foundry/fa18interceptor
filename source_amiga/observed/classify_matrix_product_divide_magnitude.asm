; Byte-exact observed matrix-product classification continuation $C2E0F8-$C2E107.
; It halves the absolute divisor comparison term, divides D0 by D3, then
; compares the absolute high quotient word with that halved term.

                org     $C2E0F8

classify_matrix_product_divide_magnitude:
                asr.w   #1,d2
                divs.w  d3,d0
                swap    d0
                tst.w   d0
                bge.b   $C2E104
                neg.w   d0
                cmp.w   d0,d2
                bgt.b   $C2E116
