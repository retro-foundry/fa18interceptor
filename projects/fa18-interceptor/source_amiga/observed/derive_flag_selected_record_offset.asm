; Byte-exact observed flag-selected record offset derivation $C14484-$C144BB.
; Bit 3 of the selected record's lead byte selects one of two signed bias
; constants for the same local-delta-to-offset calculation.

                org     $C14484

derive_flag_selected_record_offset:
                movea.l -$10(a6),a0
                move.b  (a0),d0
                btst    #3,d0
                beq.b   $C144A6
                move.w  -$16(a6),d0
                sub.w   -$1C(a6),d0
                asr.w   #8,d0
                addi.w  #$42,d0
                neg.w   d0
                move.w  d0,-$22(a6)
                bra.b   $C144D8
                move.w  -$16(a6),d0
                sub.w   -$1C(a6),d0
                asr.w   #8,d0
                addi.w  #$16,d0
                neg.w   d0
                move.w  d0,-$22(a6)
                bra.b   $C144D8
