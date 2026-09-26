; Byte-exact observed second matrix-product classification continuation
; $C2E196-$C2E1A5. It mirrors the first divide/magnitude classifier for the
; alternate shared product input.

                org     $C2E196

classify_second_matrix_product_divide_magnitude:
                asr.w   #1,d2
                divs.w  d3,d0
                swap    d0
                tst.w   d0
                bge.b   $C2E1A2
                neg.w   d0
                cmp.w   d0,d2
                bgt.b   $C2E1B4
