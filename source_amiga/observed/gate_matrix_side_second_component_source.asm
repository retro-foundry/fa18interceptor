; Byte-exact second component source gate $C136E6-$C136ED.

                org     $C136E6

gate_matrix_side_second_component_source:
                movea.l -$0E(a6),a0
                tst.w   (a0)
                dc.w    $6730                   ; beq.b $C1371E
