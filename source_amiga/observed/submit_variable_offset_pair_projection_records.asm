; Byte-exact $C2131C-$C2139D variable offset-pair projection submission.

                org     $C2131C

PROJECTION_RECORD_SELECTOR      equ     $C45954
PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_PAIR_WORKSPACE       equ     $C4C592
RECORD_RESULT_LOCAL             equ     -$7E
RECORD_TERMINATOR_LOCAL         equ     -$6E

submit_variable_offset_pair_projection_records:
                movem.l a1/a5,-(a7)
                move.w  (a2)+,PROJECTION_RECORD_SELECTOR.l
                move.w  (a2)+,d0
                clr.w   RECORD_RESULT_LOCAL(a6)
                clr.w   RECORD_TERMINATOR_LOCAL(a6)
.next_pair:
                tst.w   RECORD_TERMINATOR_LOCAL(a6)
                bne.b   .finish
                move.w  (a2)+,d1
                tst.w   (a2)
                bge.b   .second_offset
                addq.w  #1,RECORD_TERMINATOR_LOCAL(a6)
.second_offset:
                lea     PROJECTION_RECORD_TABLE.l,a3
                movem.w (a3,d0.w),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                lea     PROJECTION_PAIR_WORKSPACE.l,a0
                movem.w (a3,d1.w),d2-d4
                sub.w   d5,d2
                sub.w   d6,d3
                sub.w   d7,d4
                movem.w d2-d4,(a0)
                move.w  (a2)+,d1
                andi.w  #$7FFF,d1
                movem.w (a3,d1.w),d2-d4
                sub.w   d5,d2
                sub.w   d6,d3
                sub.w   d7,d4
                movem.w d2-d4,$6(a0)
                move.l  a2,-(a7)
                move.w  d0,-(a7)
                jsr     $C2EE4A.l
                or.w    d0,RECORD_RESULT_LOCAL(a6)
                move.w  (a7)+,d0
                movea.l (a7)+,a2
                bra.b   .next_pair
.finish:
                movem.l (a7)+,a1/a5
                move.w  RECORD_RESULT_LOCAL(a6),d0
                rts
