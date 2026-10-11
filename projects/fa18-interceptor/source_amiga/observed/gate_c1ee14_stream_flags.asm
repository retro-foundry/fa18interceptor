; Byte-exact stream-state and flag gate $C1EFB0-$C1EFC3.

                org     $C1EFB0

STREAM_STATE_FLAG               equ $C4586C

gate_c1ee14_stream_flags:
                tst.b   STREAM_STATE_FLAG.l
                bne.w   $C1EFE6
                andi.b  #$42,d6
                cmpi.b  #$42,d6
                bne.b   $C1EFE6
