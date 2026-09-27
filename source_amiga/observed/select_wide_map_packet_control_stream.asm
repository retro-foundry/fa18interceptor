; Byte-exact wide-layout map control-stream depth selector $C2AC1A-$C2AC3D.

                org     $C2AC1A

select_wide_map_packet_control_stream:
                cmpi.l  #$9000,-$28(a6)
                ble.b   $C2AC3E
                cmpi.l  #$10000,-$28(a6)
                bgt.b   .high_metric
                lea     $C2A072(pc),a0
                bra.w   $C2AD00
.high_metric:
                lea     $C2A0C2(pc),a0
                bra.w   $C2AD00
