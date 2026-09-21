; Byte-exact record-update threshold cap gate $C23C5C-$C23C61.

                org     $C23C5C

prepare_record_update_threshold_delta:
                cmpi.w  #$0900,d1
                dc.w    $6E04                   ; bgt.b $C23C66
