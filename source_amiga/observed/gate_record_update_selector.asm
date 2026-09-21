; Byte-exact record-update selector gate $C23B2C-$C23B37.

                org     $C23B2C

gate_record_update_selector:
                move.w  $6C(a1),d2
                cmpi.b  #$FF,$38(a1)
                bne.b   $C23B82
