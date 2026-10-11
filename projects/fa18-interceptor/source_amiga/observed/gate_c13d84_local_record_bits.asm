; Byte-exact observed C13D84 local-record bit gates $C14DE0-$C14E03.
; A nonzero local word selects the $C15030 route; otherwise bit 9 of one local
; record and bit 7 of another choose their respective update continuations.

                org     $C14DE0

gate_c13d84_local_record_bits:
                tst.w   -$24(a6)
                bne.w   $C15030
                movea.l -$28(a6),a0
                move.w  (a0),d0
                btst    #9,d0
                bne.w   $C14FB4
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                bne.w   $C14F06
