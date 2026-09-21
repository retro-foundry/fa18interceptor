; Byte-exact workspace component-product test $C1FBD4-$C1FC41.
; Return contract is structural: D7/condition codes mirror the final sign test.

                org     $C1FBD4

TRIPLE_WORKSPACE               equ $C4BF94

test_c1fbd4_workspace_component_product:
                lea     TRIPLE_WORKSPACE.l,a0
                movem.w (a0),d0-d5
                sub.w   d0,d3
                sub.w   d1,d4
                sub.w   d2,d5
                movem.w 12(a0),d6-d7
                sub.w   d0,d6
                sub.w   d1,d7
                move.w  16(a0),d0
                sub.w   d2,d0
                move.w  a3,d1
                asr.w   #7,d1
                andi.w  #7,d1
                beq.b   $C1FC0A
                asl.w   d1,d3
                asl.w   d1,d4
                asl.w   d1,d5
                asl.w   d1,d6
                asl.w   d1,d7
                asl.w   d1,d0
                move.w  d5,d1
                move.w  d0,d2
                muls.w  d4,d0
                muls.w  d7,d5
                sub.l   d5,d0
                asr.l   #8,d0
                muls.w  d3,d2
                muls.w  d6,d1
                sub.l   d2,d1
                asr.l   #8,d1
                muls.w  d3,d7
                muls.w  d4,d6
                sub.l   d6,d7
                asr.l   #8,d7
                muls.w  (a0)+,d0
                muls.w  (a0)+,d1
                muls.w  (a0),d7
                add.l   d0,d7
                add.l   d1,d7
                blt.b   $C1FC36
                moveq   #1,d7
                rts
                clr.w   d7
                rts
