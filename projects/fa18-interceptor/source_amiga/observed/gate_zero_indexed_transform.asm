; Byte-exact zero-index transform gate $C25DA6-$C25DAF.

                org     $C25DA6

SELECTED_RECORD_INDEX            equ $C459B4

gate_zero_indexed_transform:
                tst.w   SELECTED_RECORD_INDEX.l
                dc.w    $6762                   ; beq.b $C25E10
