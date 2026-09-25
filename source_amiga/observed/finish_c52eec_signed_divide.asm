; Byte-exact observed signed-divide loop completion $C52EEC-$C52F09.

                org     $C52EEC

C52EE0                         equ     $C52EE0
C52F04                         equ     $C52F04

finish_c52eec_signed_divide:
                dbf     d3,C52EE0
                move.l  d2,d1
                eor.l   d4,d5
                bpl.b   .positive_quotient
                neg.l   d0
.positive_quotient:
                eor.l   d1,d4
                bpl.b   C52F04
                neg.l   d1
                bra.b   C52F04
