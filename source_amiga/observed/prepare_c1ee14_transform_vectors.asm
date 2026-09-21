; Byte-exact transform-vector preparation $C1F078-$C1F0A7.

                org     $C1F078

STREAM_VECTOR_X                 equ $C45B30
STREAM_VECTOR_Y                 equ $C45B38
STREAM_VECTOR_Z                 equ $C45B3C

prepare_c1ee14_transform_vectors:
                move.l  -32(a6),d0
                move.l  -24(a6),d2
                move.l  STREAM_VECTOR_X.l,d3
                move.l  STREAM_VECTOR_Y.l,d4
                add.l   d3,d0
                add.l   d4,d2
                move.l  STREAM_VECTOR_Z.l,d1
                moveq   #8,d7
                sub.w   -6(a6),d7
                asr.l   d7,d0
                asr.l   d7,d1
                asr.l   d7,d2
                movem.w d0-d2,-20(a6)
