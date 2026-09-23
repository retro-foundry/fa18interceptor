; Byte-exact wide M-map packet-directory setup $C2AB34-$C2AB59.
; Its sibling $C2AB5A initializes the normal $C42CA8 / 16-byte layout.

                org     $C2AB34

initialize_wide_map_packet_directory:
                move.w  #1,-$3e(a6)
                clr.w   -$44(a6)
                move.l  #$C42E6C,-$34(a6)
                move.w  #8,-$40(a6)
                move.w  #$1f,-$30(a6)
                move.w  #$1f,-$2e(a6)
                bra.b   $C2AB7C
