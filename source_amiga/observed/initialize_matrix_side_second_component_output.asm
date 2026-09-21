; Byte-exact second component-output initialization $C137A2-$C137B5.

                org     $C137A2

MATRIX_SIDE_SECOND_OUTPUT        equ $C45B60

initialize_matrix_side_second_component_output:
                moveq   #0,d0
                move.w  d0,MATRIX_SIDE_SECOND_OUTPUT.l
                move.w  d0,-$1A(a6)
                movea.l -$12(a6),a0
                tst.w   (a0)
                dc.w    $670C                   ; beq.b $C137C2
