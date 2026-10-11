; Byte-exact record-update class threshold gates $C23C1C-$C23C2B.

                org     $C23C1C

INDEXED_RECORD_CLASS             equ $62

gate_record_update_class_threshold:
                cmpi.b  #$15,INDEXED_RECORD_CLASS(a1)
                beq.b   $C23C32
                dc.w    $0829,$0003,$0001       ; btst.b #3,$01(a1)
                bne.b   $C23C3E
