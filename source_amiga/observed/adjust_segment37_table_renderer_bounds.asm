; Byte-exact partially runtime-backed bounds helper $C310E2-$C3111F.

                org     $C310E2

RENDERER_BOUND_WORD              equ $C45986

adjust_segment37_table_renderer_bounds:
                move.w  RENDERER_BOUND_WORD.l,d5
                add.w   d5,d7
                bge.s   segment37_bounds_nonnegative
                neg.w   d7
                cmp.w   a4,d7
                bge.s   segment37_bounds_reject
                add.w   d7,d5
                add.w   d5,d5
                ext.l   d5
                add.l   d5,d1
                bra.s   segment37_bounds_zero_result
segment37_bounds_reject:
                moveq   #-$1,d5
                rts
segment37_bounds_nonnegative:
                add.w   d5,d5
                ext.l   d5
                add.l   d5,d1
                move.w  d7,d5
                moveq   #0,d7
                add.w   a4,d5
                subi.w  #$14,d5
                blt.s   segment37_bounds_zero_result
                cmp.w   a4,d5
                bge.s   segment37_bounds_reject
                move.w  d5,d5
                rts
                moveq   #0,d7
segment37_bounds_zero_result:
                moveq   #0,d5
                rts
