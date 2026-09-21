; Byte-exact second matrix-side threshold gate $C1353A-$C13545.

                org     $C1353A

MATRIX_SIDE_SELECTED_INDEX       equ $C461F6

compare_matrix_side_second_threshold:
                cmpi.l  #$000BB800,MATRIX_SIDE_SELECTED_INDEX.l
                dc.w    $6C24                   ; bge.b $C1356A
