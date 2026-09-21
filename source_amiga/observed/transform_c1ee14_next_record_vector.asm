; Byte-exact next-record transform and output gate $C1F21C-$C1F26B.
; The matrix/output-stream ownership remains structural.

                org     $C1F21C

STREAM_MATRIX_WORK              equ $C45BC6

transform_c1ee14_next_record_vector:
                movem.w (a1)+,d2-d4
                lea     STREAM_MATRIX_WORK.l,a2
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                movea.w d7,a4
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,d6
                muls.w  (a2)+,d2
                muls.w  (a2)+,d3
                muls.w  (a2)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                move.w  d4,d3
                subq.w  #1,-96(a6)
                blt.b   $C1F268
                move.w  a4,(a0)+
                move.w  d7,(a0)+
                move.w  d3,(a0)+
                subq.w  #1,d1
                blt.b   $C1F2E4
