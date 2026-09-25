; Byte-exact observed finish route $C153DC-$C153FB.

                org     $C153DC

RECORD_SCAN_COUNTER             equ     $C461BF
RECORD_SCAN_TABLE_END           equ     $C46201

return_c153dc_record_scan:
                tst.b   -$1(a6)
                bne.b   .return
                movea.l -$c(a6),a0
                clr.b   (a0)
                movea.l -$8(a6),a0
                move.b  (a0),d0
                andi.b  #$f,d0
                ori.b   #$50,d0
                move.b  d0,(a0)
.return:
                unlk    a6
                rts
