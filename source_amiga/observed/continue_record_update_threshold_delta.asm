; Byte-exact record-update threshold delta preparation $C23C66-$C23C75.

                org     $C23C66

continue_record_update_threshold_delta:
                add.w   d2,d1
                asr.w   #1,d1
                move.w  d1,d0
                move.w  #$0090,d3
                move.w  d3,d4
                sub.w   d2,d0
                dc.w    $6C04                   ; bge.b $C23C7A
