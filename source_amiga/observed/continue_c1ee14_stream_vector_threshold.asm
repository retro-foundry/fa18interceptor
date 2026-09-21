; Byte-exact stream-vector threshold continuation $C1EF74-$C1EF81.

                org     $C1EF74

STREAM_STAGE_SHIFT               equ $C45AB8

continue_c1ee14_stream_vector_threshold:
                move.w  STREAM_STAGE_SHIFT.l,d3
                asr.w   d3,d4
                cmp.w   -$96(a6),d4
                bge.b   $C1EF8A
