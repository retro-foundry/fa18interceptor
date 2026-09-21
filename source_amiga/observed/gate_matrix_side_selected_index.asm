; Byte-exact matrix-side selected-index gate $C134BC-$C134CB.
; The run003 frame-6000 route takes the positive branch.

                org     $C134BC

MATRIX_SIDE_SELECTED_INDEX       equ $C461F6

gate_matrix_side_selected_index:
                tst.w   -$02(a6)
                bne.w   $C13574
                tst.l   MATRIX_SIDE_SELECTED_INDEX.l
                dc.w    $6E3E                   ; bgt.b $C1350A
