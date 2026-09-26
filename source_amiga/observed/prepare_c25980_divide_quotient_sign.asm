; Byte-exact observed C25980 divide-result continuation $C25996-$C2599F.
; It halves the comparison term, performs a signed divide, exposes the high
; quotient word, and branches when that quotient is nonnegative.

                org     $C25996

prepare_c25980_divide_quotient_sign:
                asr.w   #1,d2
                divs.w  d1,d0
                swap    d0
                tst.w   d0
                bge.b   $C259A2
