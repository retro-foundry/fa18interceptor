; Byte-exact alternate-path gate $C1EFA2-$C1EFAD.

                org     $C1EFA2

STREAM_ALT_PATH_LIMIT           equ $C45A78

gate_c1ee14_stream_alt_path:
                cmpi.l  #-$140,STREAM_ALT_PATH_LIMIT.l
                ble.w   $C1EFE6
