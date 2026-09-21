; Byte-exact record-update target mode gate $C24056-$C2405D.

                org     $C24056

gate_record_update_target_mode:
                cmpi.b  #5,$7A(a1)
                bne.b   $C24064
