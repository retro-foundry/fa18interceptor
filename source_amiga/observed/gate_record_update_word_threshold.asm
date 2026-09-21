; Byte-exact record-update word threshold gate $C244E2-$C244E9.

                org     $C244E2

gate_record_update_word_threshold:
                cmpi.w  #$1200,$6C(a1)
                blt.b   $C2450E
