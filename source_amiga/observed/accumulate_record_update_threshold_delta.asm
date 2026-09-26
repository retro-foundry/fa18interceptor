; Byte-exact record-update threshold delta accumulation $C23C7E-$C23C81.
; Carries the signed threshold delta into the following record-word comparison.

                org     $C23C7E

accumulate_record_update_threshold_delta:
                add.w   d4,d2
                move.w  d2,d1
