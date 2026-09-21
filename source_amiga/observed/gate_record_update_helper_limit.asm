; Byte-exact record-update helper limit gate $C24582-$C2458D.

                org     $C24582

gate_record_update_helper_limit:
                move.b  $39(a1),d0
                andi.b  #$F0,d0
                cmp.b   d0,d1
                bge.b   $C24566
