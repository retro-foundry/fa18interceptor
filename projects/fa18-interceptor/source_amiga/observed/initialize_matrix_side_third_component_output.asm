; Byte-exact third component-output initialization $C13952-$C13975.

                org     $C13952

MATRIX_SIDE_THIRD_OUTPUT         equ $C45B62

initialize_matrix_side_third_component_output:
                moveq   #0,d0
                move.w  d0,MATRIX_SIDE_THIRD_OUTPUT.l
                movea.l -$0A(a6),a0
                move.w  (a0),d1
                move.w  d0,-$1C(a6)
                btst.l  #7,d1
                dc.w    $670E                   ; beq.b $C13978
