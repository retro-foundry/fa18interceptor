; Byte-exact runtime-backed counter gate slice $C30762-$C3076B.

                org     $C30762

SEGMENT37_COUNTER                equ $C45836

return_from_segment37_gate:
                rts

gate_segment37_counter:
                tst.b   SEGMENT37_COUNTER.l
                ble.s   return_from_segment37_gate
