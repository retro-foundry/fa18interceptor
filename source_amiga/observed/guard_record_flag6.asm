; Byte-exact observed record bit-6 guard $C2D79C-$C2D7A3.

                org     $C2D79C

                dc.w    $0829,$0006,$0003 ; btst.b #6,3(a1); retain original EA
                beq.b   $C2D7CA
