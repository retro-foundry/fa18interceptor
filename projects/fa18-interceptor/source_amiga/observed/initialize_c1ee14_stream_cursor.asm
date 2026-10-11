; Byte-exact observed stream cursor setup $C1EE40-$C1EE57.

                org     $C1EE40

STREAM_STAGE_POINTER             equ $C45A36
STREAM_STAGE_SHIFT               equ $C45AB8

initialize_c1ee14_stream_cursor:
                move.w  d1,-$28(a6)
                movea.l STREAM_STAGE_POINTER.l,a2
                move.l  a0,-$2C(a6)
                move.w  STREAM_STAGE_SHIFT.l,d3
                move.w  (a2)+,d0
                blt.b   $C1EE84
