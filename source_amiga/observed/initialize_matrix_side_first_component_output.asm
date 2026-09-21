; Byte-exact first component-output initialization $C136BE-$C136D5.

                org     $C136BE

MATRIX_SIDE_FIRST_OUTPUT         equ $C45B5E

initialize_matrix_side_first_component_output:
                moveq   #0,d0
                move.w  d0,MATRIX_SIDE_FIRST_OUTPUT.l
                movea.l -$0A(a6),a0
                move.w  (a0),d1
                move.w  d0,-$18(a6)
                btst.l  #7,d1
                dc.w    $6710                   ; beq.b $C136E6
