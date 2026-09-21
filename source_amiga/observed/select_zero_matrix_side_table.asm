; Byte-exact zero-index matrix-side table selection $C13490-$C13499.

                org     $C13490

ZERO_MATRIX_SIDE_TABLE           equ $C3D690

select_zero_matrix_side_table:
                move.l  #ZERO_MATRIX_SIDE_TABLE,-$06(a6)
                bra.b   $C134A2
