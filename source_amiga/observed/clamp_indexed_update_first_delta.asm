; Byte-exact first indexed-update delta upper-bound clamp $C25E56-$C25E59.
; The preceding negative path supplies the lower clamp at $C25E5E.

                org     $C25E56

clamp_indexed_update_first_delta:
                cmp.l   d0,d2
                ble.b   $C25E60
