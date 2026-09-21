; Byte-exact record-count gate $C1F1EE-$C1F1F5.

                org     $C1F1EE

gate_c1ee14_record_count:
                cmpi.w  #1,-98(a6)
                ble.b   $C1F21C
