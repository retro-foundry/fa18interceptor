; Byte-exact stream-descriptor continuation $C1EF8A-$C1EFA1.

                org     $C1EF8A

STREAM_STAGE_SHIFT               equ $C45AB8

continue_c1ee14_stream_descriptor:
                clr.w   d3
                move.w  d7,d2
                move.w  d2,-6(a6)
                add.w   STREAM_STAGE_SHIFT.l,d3
                move.w  d3,-8(a6)
                move.b  (a1)+,d6
                move.b  d6,-100(a6)
