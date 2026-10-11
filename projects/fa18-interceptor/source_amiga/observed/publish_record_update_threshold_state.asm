; Byte-exact record-update threshold state publication $C23C98-$C23CA5.

                org     $C23C98

publish_record_update_threshold_state:
                move.b  $65(a1),d0
                andi.b  #$FC,d0
                or.b    d2,d0
                move.b  d0,$65(a1)
