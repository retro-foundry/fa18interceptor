; Byte-exact shared adjustment return tail $C2DE7E-$C2DE95.

                org     $C2DE7E

ACTIVE_UPDATE_SELECTOR          equ     $C459B4
PRIMARY_ADJUSTMENT_OUTPUT        equ     $C45946

finish_record_depth_adjustment:
                dc.w    $4CDF,$0113 ; movem.l (a7)+,d0-d1/d4/a0
                add.w   d3,d0
                add.w   d5,d2
                tst.w   ACTIVE_UPDATE_SELECTOR.l
                bne.b   .return
                move.w  d3,PRIMARY_ADJUSTMENT_OUTPUT.l
.return:
                rts
