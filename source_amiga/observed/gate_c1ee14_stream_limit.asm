; Byte-exact stream-limit gate $C1EEA6-$C1EEB1.

                org     $C1EEA6

STREAM_STAGE_LIMIT               equ $C45A42

gate_c1ee14_stream_limit:
                cmpi.w  #$0080,STREAM_STAGE_LIMIT.l
                bge.b   $C1EED2
