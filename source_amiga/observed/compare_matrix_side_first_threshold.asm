; Byte-exact first matrix-side threshold gate $C1350A-$C13515.

                org     $C1350A

MATRIX_SIDE_SELECTED_INDEX       equ $C461F6

compare_matrix_side_first_threshold:
                cmpi.l  #$00070800,MATRIX_SIDE_SELECTED_INDEX.l
                dc.w    $6C24                   ; bge.b $C1353A
