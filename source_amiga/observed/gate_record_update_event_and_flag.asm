; Byte-exact generic record-update event/flag gates $C23AB0-$C23ABF.

                org     $C23AB0

RECORD_UPDATE_ENABLE             equ $C4578E

gate_record_update_event_and_flag:
                tst.b   RECORD_UPDATE_ENABLE.l
                dc.w    $67C2                   ; beq.b $C23A7A
                dc.w    $0829,$0001,$0020       ; btst.b #1,$20(a1)
                dc.w    $6756                   ; beq.b $C23B16
