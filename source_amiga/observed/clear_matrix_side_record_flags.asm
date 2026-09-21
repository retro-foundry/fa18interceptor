; Byte-exact matrix-side record header-flag clear and gate $C134A2-$C134BD.

                org     $C134A2

MATRIX_SIDE_CONTROL_FLAGS        equ $C458CC

clear_matrix_side_record_flags:
                movea.l -$0A(a6),a0
                move.w  (a0),d0
                andi.w  #$FFBF,d0
                move.w  d0,(a0)
                move.w  MATRIX_SIDE_CONTROL_FLAGS.l,d1
                btst.l  #6,d1
                beq.w   $C1398E
