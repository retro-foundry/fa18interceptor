; Byte-exact normal M-map packet-directory setup $C2AB5A-$C2AB7B.
; Falls through to the shared coordinate/bin setup at $C2AB7C.

                org     $C2AB5A

initialize_normal_map_packet_directory:
                clr.w   -$3e(a6)
                clr.w   -$44(a6)
                move.l  #$C42CA8,-$34(a6)
                move.w  #$c,-$40(a6)
                move.w  #7,-$30(a6)
                move.w  #7,-$2e(a6)
