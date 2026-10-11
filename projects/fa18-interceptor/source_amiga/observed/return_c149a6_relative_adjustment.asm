; Byte-exact observed finish block $C149A6-$C149BD.

                org     $C149A6

return_c149a6_relative_adjustment:
                move.w  -$6(a6),d0
                ext.l   d0
                add.l   -$a(a6),d0
                move.w  d0,-$6(a6)
                ext.l   d0
                movem.l (sp)+,d2
                unlk    a6
                rts
