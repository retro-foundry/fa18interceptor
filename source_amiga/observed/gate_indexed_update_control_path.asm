; Byte-exact indexed-update control gates $C25C54-$C25C69.

                org     $C25C54

MATRIX_SIDE_CONTROL_FLAGS        equ $C458CC
SELECTED_RECORD_INDEX            equ $C459B4

gate_indexed_update_control_path:
                move.w  MATRIX_SIDE_CONTROL_FLAGS.l,d0
                andi.w  #$0040,d0
                beq.w   $C25D86
                tst.w   SELECTED_RECORD_INDEX.l
                dc.w    $6706                   ; beq.b $C25C70
