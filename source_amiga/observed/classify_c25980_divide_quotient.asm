; Byte-exact observed C25980 divide-result continuation $C259A2-$C259AB.
; It compares the quotient magnitude with D2; within range, it restores the
; other quotient word and branches by its sign for final rounding.

                org     $C259A2

classify_c25980_divide_quotient:
                cmp.w   d0,d2
                bgt.b   $C259B4
                swap    d0
                tst.w   d0
                bge.b   $C259B0
