; Byte-exact observed map-packet continuation $C2AC76-$C2AC97.
; The selector indexes a four-byte packet entry.  A negative first byte, or
; any byte matching D2, selects the shared packet-success continuation.

                org     $C2AC76

MAP_PACKET_TABLE_SELECTOR      equ     $C45850

match_map_packet_selector_bytes:
                move.b  MAP_PACKET_TABLE_SELECTOR.l,d3
                ext.w   d3
                add.w   d3,d3
                add.w   d3,d3
                adda.w  d3,a0
                move.b  (a0)+,d4
                blt.b   $C2AC98
                cmp.b   d4,d2
                beq.b   $C2AC98
                cmp.b   (a0)+,d2
                beq.b   $C2AC98
                cmp.b   (a0)+,d2
                beq.b   $C2AC98
                cmp.b   (a0),d2
                bne.b   $C2ACAA
