; Byte-exact zero-index matrix-side return gates $C13998-$C139AB.

                org     $C13998

MATRIX_SIDE_CONTROL_FLAGS        equ $C458CC

gate_zero_matrix_side_return:
                tst.w   -$02(a6)
                bne.w   $C13A22
                move.w  MATRIX_SIDE_CONTROL_FLAGS.l,d0
                btst.l  #6,d0
                dc.w    $6764                   ; beq.b $C13A10
