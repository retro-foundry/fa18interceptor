; Byte-exact stream-descriptor flag preparation $C1EEDA-$C1EEE3.

                org     $C1EEDA

prepare_c1ee14_stream_descriptor:
                move.w  d0,d1
                moveq   #0,d7
                andi.w  #$4000,d1
                bne.b   $C1EEFA
