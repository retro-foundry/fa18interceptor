; Byte-exact observed signed stack-word adjustment $C15138-$C1518B.

                org     $C15138

adjust_signed_stack_word_pair:
                link.w  a6,#-4
                move.w  10(a6),d0
                add.w   14(a6),d0
                move.w  d0,-2(a6)
                move.w  d0,-4(a6)
                tst.w   d0
                bpl.b   .magnitude_ready
                neg.w   -4(a6)
.magnitude_ready:
                cmpi.w  #4,-4(a6)
                ble.b   .small_magnitude
                move.w  -2(a6),d0
                asr.w   #2,d0
                move.w  d0,-2(a6)
                bra.b   .apply_adjustment
.small_magnitude:
                cmpi.w  #2,-4(a6)
                ble.b   .apply_adjustment
                move.w  -2(a6),d0
                asr.w   #1,d0
                move.w  d0,-2(a6)
.apply_adjustment:
                move.w  -2(a6),d0
                sub.w   14(a6),d0
                move.w  d0,10(a6)
                ext.l   d0
                unlk    a6
                rts
