; Byte-exact indexed-transform control gate $C25E10-$C25E1B.

                org     $C25E10

MATRIX_SIDE_CONTROL_FLAGS        equ $C458CC

gate_indexed_transform_control:
                move.w  MATRIX_SIDE_CONTROL_FLAGS.l,d0
                andi.w  #$0040,d0
                dc.w    $6608                   ; bne.b $C25E24
