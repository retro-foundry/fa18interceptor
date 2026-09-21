; Byte-exact accumulated-vector limit gate $C1F1B6-$C1F1BD.

                org     $C1F1B6

STREAM_VECTOR_LIMIT             equ $C459C0

gate_c1ee14_accumulated_vector_limit:
                move.w  STREAM_VECTOR_LIMIT.l,d2
                blt.b   $C1F1EE
