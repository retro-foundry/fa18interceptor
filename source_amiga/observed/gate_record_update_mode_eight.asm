; Byte-exact record-update mode-eight gate $C23CB0-$C23CB7.

                org     $C23CB0

gate_record_update_mode_eight:
                cmpi.b  #8,$05(a1)
                bne.w   $C23D3C
