; Byte-exact observed-entry slice $C305AA-$C305D5 (Hunk 36 +$119A).
; The equal D1/D3 path returns; unequal cases prepare a continuation state.

                org     $C305AA

LOW_OR_EQUAL_CONTINUATION      equ $C305D6
UNEQUAL_RENDERER_CONTINUATION  equ $C305F8

prepare_unequal_pair_range:
                cmp.w   d1,d3
                beq.s   .return
                bls.s   LOW_OR_EQUAL_CONTINUATION
                move.w  d3,d5
                sub.w   d1,d5
                subq.w  #1,d5
                addq.w  #1,d1
                cmp.w   a4,d1
                bgt.s   .return
                movea.w d1,a1
                asl.w   #3,d1
                move.w  d1,d7
                add.w   d7,d7
                add.w   d7,d7
                add.w   d1,d7
                move.w  d2,d4
                sub.w   d0,d4
                move.w  d0,d6
                lsr.w   #3,d0
                add.w   d0,d7
                bra.s   UNEQUAL_RENDERER_CONTINUATION
.return:
                rts