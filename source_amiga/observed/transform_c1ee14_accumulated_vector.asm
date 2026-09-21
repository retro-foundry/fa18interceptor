; Byte-exact accumulated-vector transform $C1F160-$C1F1B5.
; Matrix and output-stream ownership remain structural.

                org     $C1F160

STREAM_ACCUMULATED_MATRIX       equ $C45BD8

transform_c1ee14_accumulated_vector:
                move.w  a4,d2
                move.w  d6,d3
                move.w  -8(a6),d7
                asr.w   d7,d2
                asr.w   d7,d3
                asr.w   d7,d4
                add.w   -20(a6),d2
                add.w   -18(a6),d3
                add.w   -16(a6),d4
                lea     STREAM_ACCUMULATED_MATRIX.l,a2
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  (a2)+,d3
                muls.w  (a2)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                move.w  d4,(a3)+
