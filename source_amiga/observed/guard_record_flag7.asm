; Byte-exact observed record bit-7 guard $C2D7E6-$C2D7EF.

                org     $C2D7E6

                dc.w    $0829,$0007,$0003 ; btst.b #7,3(a1); retain original EA
                bne.w   $C2D8A8
