; Byte-exact fixed-point change gate $C25EC8-$C25ECB.

                org     $C25EC8

gate_indexed_update_fixed_point_change:
                tst.b   d6
                dc.w    $6706                   ; beq.b $C25ED2
