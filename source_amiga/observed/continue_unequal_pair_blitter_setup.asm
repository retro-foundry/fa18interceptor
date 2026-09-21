; Byte-exact runtime-backed continuation $C305D6-$C30667 of C305AA.

                org     $C305D6

UNEQUAL_PAIR_RETURN             equ $C305D4
RENDERER_POINTER_9              equ $C456E2

continue_unequal_pair_blitter_setup:
                move.w  d1,d5
                sub.w   d3,d5
                subq.w  #1,d5
                move.w  d0,d4
                sub.w   d2,d4
                addq.w  #1,d3
                cmp.w   a4,d3
                bgt.s   UNEQUAL_PAIR_RETURN
                movea.w d3,a1
                asl.w   #3,d3
                move.w  d3,d7
                add.w   d7,d7
                add.w   d7,d7
                add.w   d3,d7
                move.w  d2,d6
                lsr.w   #3,d2
                add.w   d2,d7
                ext.l   d7
                add.l   RENDERER_POINTER_9.l,d7
                moveq   #3,d1
                andi.w  #$F,d6
                ror.w   #4,d6
                addi.w  #$B4A,d6
                tst.w   d4
                bmi.s   .negative_delta
                cmp.w   d5,d4
                bcs.s   .swap_delta
                addi.w  #$10,d1
                bra.s   .fit_delta
.negative_delta:
                neg.w   d4
                cmp.w   d5,d4
                bcs.s   .negative_swap
                addi.w  #$14,d1
.fit_delta:
                suba.w  a1,a4
                cmp.w   a4,d5
                ble.s   .direct_limit
                move.w  a4,d2
                move.w  d4,d3
                muls.w  d2,d3
                add.l   d3,d3
                divs.w  d5,d3
                asr.w   #1,d3
                bcc.s   .rounded_limit
                addq.w  #1,d3
.rounded_limit:
                movea.w d3,a4
                bra.s   .finish_limit
.negative_swap:
                addq.w  #8,d1
.swap_delta:
                exg     d4,d5
                suba.w  a1,a4
                cmp.w   a4,d4
                bgt.s   .finish_limit
.direct_limit:
                movea.w d4,a4
.finish_limit:
                add.w   d5,d5
                add.w   d5,d5
                add.w   d4,d4
                move.w  d5,d2
                sub.w   d4,d2
                bge.s   .no_sign_bit
                bset    #6,d1
.no_sign_bit:
                move.w  d5,d3
                add.w   d4,d4
                sub.w   d4,d5
                move.w  a4,d4
                asl.w   #6,d4
                addi.w  #$42,d4
