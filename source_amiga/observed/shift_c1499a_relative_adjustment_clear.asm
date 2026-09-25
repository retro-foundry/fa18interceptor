; Byte-exact observed bit-3-clear block $C1499A-$C149A5.

                org     $C1499A

shift_c1499a_relative_adjustment_clear:
                moveq   #$d,d0
                move.l  -$a(a6),d1
                asr.l   d0,d1
                move.l  d1,-$a(a6)
