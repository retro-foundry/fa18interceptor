; Byte-exact alternate-branch transform loop $C1F524-$C1F577.
; Matrix/output-stream ownership remains structural.

                org     $C1F524

transform_c1ee14_alt_branch_loop:
                tst.w   (a0)
                ble.b   $C1F578
                movem.w (a1)+,d2-d4
                move.w  -8(a6),d7
                asr.w   d7,d2
                asr.w   d7,d3
                asr.w   d7,d4
                add.w   d0,d2
                add.w   a5,d3
                add.w   d1,d4
                lea     (a4),a2
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
                subq.w  #1,(a0)
                bgt.b   $C1F528
