; Byte-exact observed bit-3-set block $C14990-$C14999.

                org     $C14990

RELATIVE_ADJUSTMENT_JOIN        equ     $C149A6

shift_c14990_relative_adjustment_bit3:
                moveq   #$b,d0
                asr.l   d0,d1
                move.l  d1,-$a(a6)
                bra.b   RELATIVE_ADJUSTMENT_JOIN
