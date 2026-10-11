; Byte-exact second indexed-update delta clamp gate $C25E60-$C25E67.

                org     $C25E60

clamp_indexed_update_second_delta:
                add.l   d7,d4
                dc.w    $6D08                   ; blt.b $C25E6C
                cmp.l   d0,d4
                dc.w    $6F06                   ; ble.b $C25E6E
