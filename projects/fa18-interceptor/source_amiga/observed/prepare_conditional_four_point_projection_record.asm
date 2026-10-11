; Byte-exact $C21500-$C2159D conditional four-point projection preparation.

                org     $C21500

PROJECTION_RECORD_SELECTOR      equ     $C45954
PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_QUAD_WORKSPACE       equ     $C4BF90
PROJECTION_DEPTH                equ     $C45A78

prepare_conditional_four_point_projection_record:
                move.w  (a2)+,PROJECTION_RECORD_SELECTOR.l
                lea     PROJECTION_RECORD_TABLE.l,a3
                adda.w  (a2)+,a3
                cmpi.l  #-$80,PROJECTION_DEPTH.l
                blt.b   $C214FC
                movem.l a1-a2/a5,-(a7)
                lea     PROJECTION_QUAD_WORKSPACE.l,a0
                clr.w   (a0)+
                move.w  #4,(a0)+
                movem.w (a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movea.w d5,a1
                movea.w d6,a4
                movea.w d7,a5
                movem.w $12(a3),d0-d2
                move.w  d0,(a0)+
                move.w  d1,(a0)+
                move.w  d2,(a0)+
                add.w   d5,d0
                add.w   d6,d1
                add.w   d7,d2
                move.w  d0,(a0)+
                move.w  d1,(a0)+
                move.w  d2,(a0)+
                movea.w d2,a2
                movem.w $6(a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                move.w  a2,d2
                add.w   d5,d0
                add.w   d6,d1
                add.w   d7,d2
                move.w  d0,(a0)+
                move.w  d1,(a0)+
                move.w  d2,(a0)+
                sub.w   a1,d0
                sub.w   a4,d1
                sub.w   a5,d2
                movem.w d0-d2,(a0)
                move.w  -$E(a0),d7
                and.w   -$8(a0),d7
                and.w   -$2(a0),d7
                and.w   $4(a0),d7
                blt.b   .reject_saved
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.reject_saved:
                movem.l (a7)+,a1-a2/a5
