; Byte-exact record-adjustment below-$3840 scale preparation $C2DE56-$C2DE5B.

                org     $C2DE56

prepare_record_adjustment_scale_below_3840:
                subi.w  #$7080,d1
                neg.w   d1
