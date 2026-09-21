; Byte-exact $C2122A-$C2129B counted offset-projection submission.

                org     $C2122A

PROJECTION_RECORD_SELECTOR      equ     $C45954
PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_PAIR_WORKSPACE       equ     $C4C592
RECORD_COUNT_LOCAL              equ     -$30
RECORD_RESULT_LOCAL             equ     -$7E

submit_counted_offset_projection_records:
                move.w  (a2)+,d0
                move.w  d0,d1
                andi.w  #$3F,d1
                move.w  d1,PROJECTION_RECORD_SELECTOR.l
                lsr.w   #8,d0
                move.w  d0,RECORD_COUNT_LOCAL(a6)
                move.w  (a2)+,d0
                lea     PROJECTION_RECORD_TABLE.l,a3
                movem.w (a3,d0.w),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movea.w d5,a4
                adda.w  (a2)+,a3
                movem.l a1-a2/a5,-(a7)
                clr.w   RECORD_RESULT_LOCAL(a6)
.submit_next_record:
                movem.w (a3)+,d0-d5
                sub.w   a4,d0
                sub.w   d6,d1
                sub.w   d7,d2
                sub.w   a4,d3
                sub.w   d6,d4
                sub.w   d7,d5
                movem.w d0-d5,PROJECTION_PAIR_WORKSPACE.l
                move.l  a3,-(a7)
                movem.w d6-d7/a4,-(a7)
                jsr     $C2EE4A.l
                or.w    d0,RECORD_RESULT_LOCAL(a6)
                movem.w (a7)+,d6-d7/a4
                movea.l (a7)+,a3
                subq.w  #1,RECORD_COUNT_LOCAL(a6)
                bgt.b   .submit_next_record
                movem.l (a7)+,a1-a2/a5
                move.w  RECORD_RESULT_LOCAL(a6),d0
                rts
