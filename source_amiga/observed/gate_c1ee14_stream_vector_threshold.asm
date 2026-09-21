; Byte-exact stream-vector threshold gate $C1EF62-$C1EF6D.

                org     $C1EF62

gate_c1ee14_stream_vector_threshold:
                move.w  d7,-$04(a6)
                asr.w   d3,d5
                cmp.w   -$96(a6),d5
                bge.b   $C1EF74
