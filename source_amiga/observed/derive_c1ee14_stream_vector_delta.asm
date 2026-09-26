; Byte-exact C1EE14 stream-vector delta path $C1EF82-$C1EF89.

                org     $C1EF82

derive_c1ee14_stream_vector_delta:
                move.w  d6,d3
                move.w  d7,d2
                sub.w   d6,d2
                bra.b   $C1EF8E
