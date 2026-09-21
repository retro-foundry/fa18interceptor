; Byte-exact stream descriptor vector load/gate $C1EF2E-$C1EF43.

                org     $C1EF2E

STREAM_STAGE_VECTOR              equ $C45A62
STREAM_STAGE_SHIFT               equ $C45AB8

load_c1ee14_stream_vector:
                movem.l STREAM_STAGE_VECTOR.l,d0-d2
                move.w  STREAM_STAGE_SHIFT.l,d3
                beq.b   $C1EF44
