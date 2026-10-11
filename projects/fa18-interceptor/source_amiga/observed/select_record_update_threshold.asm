; Byte-exact default record-update threshold selection $C23C2C-$C23C31.

                org     $C23C2C

select_record_update_threshold:
                move.w  #$1D40,d1
                bra.b   $C23C5C
