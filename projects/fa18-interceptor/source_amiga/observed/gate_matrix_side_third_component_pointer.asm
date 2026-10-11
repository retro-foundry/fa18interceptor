; Byte-exact third component pointer gate $C13978-$C1397F.

                org     $C13978

gate_matrix_side_third_component_pointer:
                movea.l -$16(a6),a0
                tst.w   (a0)
                dc.w    $6718                   ; beq.b $C13998
