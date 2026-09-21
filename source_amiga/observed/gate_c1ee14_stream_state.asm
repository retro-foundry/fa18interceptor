; Byte-exact stream-state gate $C1EED2-$C1EED9.

                org     $C1EED2

STREAM_STAGE_STATE               equ $C45864

gate_c1ee14_stream_state:
                tst.b   STREAM_STAGE_STATE.l
                beq.b   $C1EE9A
