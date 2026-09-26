; Byte-exact mode-dispatch continuation $C09E3C-$C09E97.

                org     $C09E3C

DISPATCH_RETURN                 equ     $C09E94

                cmpi.b  #3,d0
                bne.b   .mode_four
                bsr.w   $C09E98
                bra.b   DISPATCH_RETURN
.mode_four:
                cmpi.b  #4,d0
                bne.b   .mode_five
                bsr.w   $C09EC4
                bra.b   DISPATCH_RETURN
.mode_five:
                cmpi.b  #5,d0
                bne.b   .mode_six
                bsr.w   $C0A002
                bra.b   DISPATCH_RETURN
.mode_six:
                cmpi.b  #6,d0
                bne.b   .mode_seven
                bsr.w   $C0A15C
                bra.b   DISPATCH_RETURN
.mode_seven:
                cmpi.b  #7,d0
                bne.b   .mode_nine
                bsr.w   $C0A1E0
                bra.b   DISPATCH_RETURN
.mode_nine:
                cmpi.b  #9,d0
                bne.b   .mode_7d
                bsr.w   $C0A2F0
                bra.b   DISPATCH_RETURN
.mode_7d:
                cmpi.b  #$7D,d0
                bne.b   .remaining
                bsr.w   $C0A334
                bra.b   DISPATCH_RETURN
.remaining:
                bsr.w   $C0A364
                rts

                rts
