; Byte-exact observed post-matrix class-$10 guard $C2D7F0-$C2D7FF.

                org     $C2D7F0

                move.b  $62(a1),d0
                andi.b  #$f0,d0
                cmpi.b  #$10,d0
                bne.w   $C2D8A8
