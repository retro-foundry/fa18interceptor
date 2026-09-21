; Byte-exact observed stream-stage entry $C1EE14-$C1EE2D.

                org     $C1EE14

STREAM_STAGE_WORD                equ $C45B40
STREAM_STAGE_EVENT               equ $C457DD

initialize_c1ee14_stream_stage:
                link    a6,#-$98
                clr.w   -$80(a6)
                move.w  STREAM_STAGE_WORD.l,d1
                move.w  d1,-$96(a6)
                tst.b   STREAM_STAGE_EVENT.l
                bne.b   $C1EE40
