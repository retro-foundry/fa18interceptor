; Byte-exact $C211DC-$C21229 counted six-word projection-record submitter.

                org     $C211DC

RENDER_SELECTOR                 equ     $C45954
PROJECTED_VERTEX_TABLE          equ     $C48390
PROJECTED_PAIR_WORKSPACE        equ     $C4C592
RECORD_COUNT_LOCAL              equ     -$30
RENDER_STATUS_LOCAL             equ     -$7E
PREPARE_PROJECTED_SEGMENT       equ     $C2EE4A

submit_counted_consecutive_projection_records:
                move.w  (a2)+,d0
                move.w  d0,d1
                andi.w  #$003F,d1
                move.w  d1,RENDER_SELECTOR.l
                lsr.w   #8,d0
                move.w  d0,RECORD_COUNT_LOCAL(a6)
                lea     PROJECTED_VERTEX_TABLE.l,a3
                adda.w  (a2)+,a3
                movem.l a1-a2/a5,-(a7)
                clr.w   RENDER_STATUS_LOCAL(a6)
.record_loop:
                movem.w (a3)+,d0-d5
                movem.w d0-d5,PROJECTED_PAIR_WORKSPACE.l
                move.l  a3,-(a7)
                jsr     PREPARE_PROJECTED_SEGMENT.l
                or.w    d0,RENDER_STATUS_LOCAL(a6)
                movea.l (a7)+,a3
                subq.w  #1,RECORD_COUNT_LOCAL(a6)
                bgt.b   .record_loop
                movem.l (a7)+,a1-a2/a5
                move.w  RENDER_STATUS_LOCAL(a6),d0
                rts
