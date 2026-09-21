; Byte-exact stream-descriptor field decode $C1EF16-$C1EF2D.

                org     $C1EF16

decode_c1ee14_stream_descriptor:
                move.w  (a1)+,d0
                move.b  d0,-$88(a6)
                move.w  (a1)+,d4
                move.w  (a1)+,d5
                move.b  (a1)+,d6
                move.b  d6,d7
                asr.b   #4,d6
                andi.w  #$000F,d6
                andi.w  #$000F,d7
