; Byte-exact $C2139E-$C21411 direct four-point projection preparation.

                org     $C2139E

PROJECTION_RECORD_SELECTOR      equ     $C45954
PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_QUAD_WORKSPACE       equ     $C4BF90

prepare_direct_four_point_projection_record:
                move.w  (a2)+,PROJECTION_RECORD_SELECTOR.l
                movem.w (a2)+,d0-d2/d7
                lea     PROJECTION_RECORD_TABLE.l,a3
                lea     PROJECTION_QUAD_WORKSPACE.l,a0
                clr.w   (a0)+
                move.w  #4,(a0)+
                move.l  (a3,d1.w),(a0)+
                move.w  $4(a3,d1.w),(a0)+
                move.w  -$2(a0),d6
                move.l  (a3,d2.w),(a0)+
                move.w  $4(a3,d2.w),(a0)+
                and.w   -$2(a0),d6
                move.l  (a3,d7.w),(a0)+
                move.w  $4(a3,d7.w),(a0)+
                and.w   -$2(a0),d6
                movem.w (a3,d0.w),d0-d5
                sub.w   d0,d3
                sub.w   d1,d4
                sub.w   d2,d5
                movem.w (a3,d7.w),d0-d2
                sub.w   d3,d0
                sub.w   d4,d1
                sub.w   d5,d2
                movem.w d0-d2,(a0)
                and.w   d2,d6
                blt.b   .reject
                movem.l a1-a2/a5,-(a7)
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.reject:
                moveq   #0,d0
                rts
