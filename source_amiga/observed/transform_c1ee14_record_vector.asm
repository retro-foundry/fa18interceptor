; Byte-exact record-vector transform $C1F100-$C1F14F.
; The matrix and vector ownership remain structural.

                org     $C1F100

STREAM_MATRIX_WORK              equ $C45BC6

transform_c1ee14_record_vector:
                move.w  #$39,-96(a6)
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
                move.w  a4,(a0)+
                move.w  d7,(a0)+
                move.w  d4,(a0)+
                tst.w   d1
                blt.w   $C1F2E4
