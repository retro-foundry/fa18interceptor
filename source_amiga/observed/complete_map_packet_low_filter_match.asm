; Byte-exact successful low-metric map packet filter continuation $C2AC98-$C2ACA7.

                org     $C2AC98

complete_map_packet_low_filter_match:
                lea.l   $C2ACA8.l,a0
                move.b  #1,$C4589D.l
                bra.b   $C2AD00

                org     $C2ACA8
                dc.w    $04FF
