; Byte-exact map-packet workspace threshold gate $C2AC1A-$C2AC23.

                org     $C2AC1A

gate_map_packet_workspace_9000:
                cmpi.l  #$9000,-$28(a6)
                ble.b   $C2AC3E
