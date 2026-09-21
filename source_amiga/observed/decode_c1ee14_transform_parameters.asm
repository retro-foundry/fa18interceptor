; Byte-exact transform parameter decode $C1F0EE-$C1F0FF.

                org     $C1F0EE

decode_c1ee14_transform_parameters:
                move.b  (a1)+,d0
                ext.w   d0
                move.b  (a1)+,d1
                ext.w   d1
                subq.w  #1,d0
                subq.w  #1,d1
                sub.w   d1,d0
                blt.w   $C1F2D4
