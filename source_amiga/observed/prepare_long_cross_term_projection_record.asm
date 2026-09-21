; Byte-exact $C2168A-$C217E9 conditional projection-record preparation.

                org     $C2168A

PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_TRI_WORKSPACE        equ     $C4BF92
PROJECTION_RECORD_SELECTOR      equ     $C45954
RECORD_COMPONENT_BASE           equ     $C45A32
RECORD_COMPONENT_X              equ     $C45B2A
RECORD_COMPONENT_Y              equ     $C45B2E
RECORD_COMPONENT_SHIFT          equ     $C45AB8
NEGATED_COMPONENT_X             equ     $C45A72
NEGATED_COMPONENT_Y             equ     $C45A76

prepare_long_cross_term_projection_record:
                lea     PROJECTION_RECORD_TABLE.l,a3
                lea     PROJECTION_TRI_WORKSPACE.l,a0
                move.w  #3,(a0)+
                move.w  (a2)+,PROJECTION_RECORD_SELECTOR.l
                move.w  (a2)+,d0
                movem.w (a3,d0.w),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                swap    d6
                swap    d7
                movea.l RECORD_COMPONENT_BASE.l,a4
                move.w  (a2)+,d0
                move.w  $A(a4,d0.w),d1
                move.w  $E(a4,d0.w),d2
                move.w  d1,d3
                move.w  d2,d4
                move.w  (a2)+,d0
                move.w  d0,d6
                bclr    #$F,d0
                sub.w   $A(a4,d0.w),d1
                sub.w   $E(a4,d0.w),d2
                move.b  $6(a4),d7
                andi.w  #$F,d7
                asr.w   d7,d3
                asr.w   d7,d4
                add.w   RECORD_COMPONENT_X.l,d3
                add.w   RECORD_COMPONENT_Y.l,d4
                move.w  RECORD_COMPONENT_SHIFT.l,d7
                asl.w   d7,d3
                asl.w   d7,d4
                add.w   NEGATED_COMPONENT_X.l,d3
                add.w   NEGATED_COMPONENT_Y.l,d4
                muls.w  d1,d3
                muls.w  d2,d4
                add.l   d3,d4
                swap    d6
                swap    d7
                move.w  (a2)+,d0
                movem.w (a3,d0.w),d1-d3
                sub.w   d5,d1
                sub.w   d6,d2
                sub.w   d7,d3
                move.w  d1,(a0)+
                move.w  d2,(a0)+
                move.w  d3,(a0)+
                adda.w  (a2)+,a3
                movem.l a1-a2/a5,-(a7)
                tst.l   d4
                bge.b   .nonnegative_cross_term
                tst.l   d6
                blt.b   .table_layout
                bra.b   .build_adjusted_layout
.reject_early:
                movem.l (a7)+,a1-a2/a5
                moveq   #0,d0
                rts
.nonnegative_cross_term:
                tst.l   d6
                blt.b   .build_adjusted_layout
.table_layout:
                movem.w (a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movem.w $12(a3),d0-d2
                move.w  d0,(a0)+
                move.w  d1,(a0)+
                move.w  d2,(a0)+
                add.w   d5,d0
                add.w   d6,d1
                add.w   d7,d2
                movea.w d2,a2
                movem.w $6(a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                move.w  a2,d2
                add.w   d5,d0
                add.w   d6,d1
                add.w   d7,d2
                movem.w d0-d2,(a0)
                and.w   -$8(a0),d2
                and.w   -$2(a0),d2
                blt.b   .reject_early
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.build_adjusted_layout:
                movem.w (a3),d2-d7
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movea.w d5,a1
                movea.w d6,a4
                movea.w d7,a5
                movem.w $12(a3),d0-d2
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
                sub.w   a1,d0
                sub.w   a4,d1
                sub.w   a5,d2
                movem.w d0-d2,(a0)
                and.w   -$8(a0),d2
                and.w   -$2(a0),d2
                blt.b   .reject_tail
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.reject_tail:
                movem.l (a7)+,a1-a2/a5
                moveq   #0,d0
                rts
