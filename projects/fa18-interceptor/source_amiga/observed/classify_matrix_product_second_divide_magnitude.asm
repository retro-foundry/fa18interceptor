; Byte-exact observed fixed-point second-divide magnitude classification
; $C2E224-$C2E22D. D6 is the halved absolute divisor magnitude; the quotient
; magnitude selects the adjustment path below.

                org     $C2E224

classify_matrix_product_second_divide_magnitude:
                asr.w   #1,d6
                divs.w  d3,d0
                swap    d0
                tst.w   d0
                bge.b   $C2E230
