; Byte-exact matrix-side record-path epilogue $C13A22-$C13A29.

                org     $C13A22

finish_matrix_side_record_path:
                movem.l (a7)+,d2-d3/a2-a3
                unlk    a6
                rts
