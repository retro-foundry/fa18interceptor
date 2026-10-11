; Byte-exact record-update threshold delta gate $C23C7A-$C23C7D.

                org     $C23C7A

gate_record_update_threshold_delta:
                cmp.w   d3,d0
                dc.w    $6D04                   ; blt.b $C23C82
