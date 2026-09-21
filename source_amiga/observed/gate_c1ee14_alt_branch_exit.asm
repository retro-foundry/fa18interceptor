; Byte-exact alternate-branch exit flag gate $C1F578-$C1F583.

                org     $C1F578

gate_c1ee14_alt_branch_exit:
                move.b  -100(a6),d4
                andi.b  #8,d4
                beq.w   $C1F6F8
