; Byte-exact selected-status gates $C139AC-$C139BF.

                org     $C139AC

MATRIX_SIDE_SELECTED_INDEX       equ $C461F6
MATRIX_SIDE_SELECTED_FLAGS       equ $C461A4

gate_matrix_side_selected_status:
                tst.l   MATRIX_SIDE_SELECTED_INDEX.l
                dc.w    $675C                   ; beq.b $C13A10
                move.b  MATRIX_SIDE_SELECTED_FLAGS.l,d0
                btst.l  #2,d0
                dc.w    $673C                   ; beq.b $C139FC
