; Byte-exact candidate offset advance $C26EFC-$C26F09.

                org     $C26EFC

                move.b  d6,-$22(a6)
                addi.w  #$200,d0
                cmpi.w  #$1e00,d0
                bgt.w   $C27504
