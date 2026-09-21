; Byte-exact matrix-side status-mask clear $C1356A-$C13573.

                org     $C1356A

MATRIX_SIDE_STATUS               equ $C45B50

clear_matrix_side_status_mask:
                andi.l  #$FFFFFFBC,MATRIX_SIDE_STATUS.l
