; Byte-exact selected-record gates $C25D22-$C25D3F.

                org     $C25D22

SELECTED_RECORD_INDEX            equ $C459B4
INDEXED_UPDATE_REFERENCE_INDEX   equ $C458DC
INDEXED_UPDATE_CONTEXT           equ $C45785

gate_indexed_update_selected_record:
                move.w  SELECTED_RECORD_INDEX.l,d0
                cmp.w   INDEXED_UPDATE_REFERENCE_INDEX.l,d0
                bne.b   $C25D5E
                tst.b   INDEXED_UPDATE_CONTEXT.l
                bne.b   $C25D5E
                btst.b  #0,$03(a1)
                dc.w    $671E                   ; beq.b $C25D5E
