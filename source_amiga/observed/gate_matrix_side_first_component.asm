; Byte-exact first matrix-side component gate $C135AC-$C135B3.
; The bounded trace takes the final BEQ.

                org     $C135AC

gate_matrix_side_first_component:
                tst.w   -$18(a6)
                beq.w   $C136BE
