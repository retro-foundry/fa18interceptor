; Byte-exact matrix-side status-bit gate $C139FC-$C13A05.

                org     $C139FC

MATRIX_SIDE_STATUS_BYTE          equ $C45B53

gate_matrix_side_status_bit:
                btst.b  #7,MATRIX_SIDE_STATUS_BYTE.l
                dc.w    $671C                   ; beq.b $C13A22
