; Byte-exact observed map-packet continuation $C2ABD2-$C2ABDD.
; It saves D0-D1 into the local workspace and tests local -$3E before the
; alternate map-packet route.

                org     $C2ABD2

gate_map_packet_local_mode:
                movem.w d0-d1,-$2c(a6)
                tst.w   -$3e(a6)
                bne.b   $C2AC1A
