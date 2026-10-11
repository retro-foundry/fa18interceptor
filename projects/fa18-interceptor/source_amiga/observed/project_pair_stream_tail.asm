; Byte-exact observed projection-stream tail $C07840-$C0795F.
; It consumes signed word pairs from A1, applies an A4 matrix-like coefficient
; block, and emits three-word tuples through A3.  Record ownership is unknown.

                org     $C07840

PROJECT_STREAM_EXIT             equ $C1F6F8

project_pair_stream_tail:
                jmp     PROJECT_STREAM_EXIT.l
.read_outer_count:
                move.w  (a1)+,-$30(a6)
                blt.s   project_pair_stream_tail
                move.w  d3,-(sp)
                move.w  d6,-(sp)
                movem.w -$c(a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movem.w d5-d7,-$58(a6)
                move.w  (sp)+,d6
                move.w  (sp)+,d3
.project_loop_pair:
                move.w  (a1)+,d2
                move.w  (a1)+,d4
                move.w  -$8(a6),d7
                beq.s   .pair_unshifted
                asr.w   d7,d2
                asr.w   d7,d4
.pair_unshifted:
                add.w   d0,d2
                add.w   d1,d4
                lea     (a4),a2
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d3,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  2(a2),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   a5,d4
                move.w  d4,(a3)+
                move.w  -6(a3),d2
                exg     d7,d4
                add.w   -$58(a6),d2
                add.w   -$56(a6),d4
                add.w   -$54(a6),d7
                movem.w d2/d4/d7,(a3)
                addq.w  #6,a3
                subq.w  #1,-$30(a6)
                bgt.s   .project_loop_pair
                tst.w   (a1)+
                blt.w   project_pair_stream_tail
.project_first_tail_pair:
                move.w  (a1)+,d2
                move.w  (a1)+,d4
                move.w  -$8(a6),d7
                beq.s   .first_tail_unshifted
                asr.w   d7,d2
                asr.w   d7,d4
.first_tail_unshifted:
                add.w   d0,d2
                add.w   d1,d4
                lea     (a4),a2
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d3,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  2(a2),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   a5,d4
                move.w  d4,(a3)+
.project_second_tail_pair:
                move.w  (a1)+,d2
                move.w  (a1)+,d4
                move.w  -$8(a6),d7
                beq.s   .second_tail_unshifted
                asr.w   d7,d2
                asr.w   d7,d4
.second_tail_unshifted:
                add.w   d0,d2
                add.w   d1,d4
                lea     (a4),a2
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d3,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  2(a2),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   a5,d4
                move.w  d4,(a3)+
                bra.w   .read_outer_count
