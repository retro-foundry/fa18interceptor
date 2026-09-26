; Byte-exact observed map-packet continuation $C2AC3E-$C2AC65.
; It sign-extends a shared byte selector and classifies local -$28 against
; four observed signed thresholds before choosing the adjacent table routes.

                org     $C2AC3E

MAP_PACKET_METRIC_SELECTOR     equ     $C45854

classify_map_packet_workspace_metric:
                move.b  MAP_PACKET_METRIC_SELECTOR.l,d2
                ext.w   d2
                cmpi.l  #$3a0,-$28(a6)
                bgt.w   $C2ACAA
                cmpi.l  #$160,-$28(a6)
                bgt.b   $C2AC72
                cmpi.l  #$10,-$28(a6)
                bgt.b   $C2AC6C
