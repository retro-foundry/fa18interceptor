; Byte-exact indexed-update record flag gate $C25C3E-$C25C45.

                org     $C25C3E

gate_indexed_update_record_flag:
                btst.b  #0,$02(a1)
                dc.w    $670E                   ; beq.b $C25C54
