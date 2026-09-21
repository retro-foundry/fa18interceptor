; Byte-exact record-update low threshold gate $C23C0C-$C23C13.

                org     $C23C0C

gate_record_update_low_threshold:
                cmpi.w  #$0300,$4A(a1)
                dc.w    $6C08                   ; bge.b $C23C1C
