; Byte-exact $C2159E-$C21689 conditional three-point projection preparation.

                org     $C2159E

PROJECTION_RECORD_TABLE         equ     $C48390
PROJECTION_TRI_WORKSPACE        equ     $C4BF92
PROJECTION_RECORD_SELECTOR      equ     $C45954
RECORD_COMPONENT_BASE           equ     $C45A32
RECORD_COMPONENT_X              equ     $C45B2A
RECORD_COMPONENT_Y              equ     $C45B2E
RECORD_COMPONENT_SHIFT          equ     $C45AB8
NEGATED_COMPONENT_X             equ     $C45A72
NEGATED_COMPONENT_Y             equ     $C45A76

prepare_conditional_three_point_projection_record:
                lea     PROJECTION_RECORD_TABLE.l,a3
                lea     PROJECTION_TRI_WORKSPACE.l,a0
                move.w  #3,(a0)+
                move.w  (a2)+,PROJECTION_RECORD_SELECTOR.l
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
                move.w  (a2)+,d0
                move.l  (a3,d0.w),(a0)+
                move.w  $4(a3,d0.w),(a0)+
                adda.w  (a2)+,a3
                movem.l a1-a2/a5,-(a7)
                add.l   d3,d4
                bge.b   .positive_cross_term
                tst.w   d6
                blt.b   .build_from_table
                bra.b   .submit_or_reject
.positive_cross_term:
                tst.w   d6
                blt.b   .submit_or_reject
.build_from_table:
                move.l  (a3),(a0)+
                move.w  $4(a3),(a0)+
                move.l  $C(a3),(a0)+
                move.w  $10(a3),(a0)
                move.w  -$C(a0),d7
                and.w   -$6(a0),d7
                and.w   (a0),d7
                blt.b   .reject
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.submit_or_reject:
                movem.w (a3)+,d2-d7
                move.w  d5,(a0)+
                move.w  d6,(a0)+
                move.w  d7,(a0)+
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movem.w (a3),d2-d4
                sub.w   d5,d2
                sub.w   d6,d3
                sub.w   d7,d4
                movem.w d2-d4,(a0)
                and.w   -$8(a0),d4
                and.w   -$2(a0),d4
                blt.b   .reject
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.reject:
                movem.l (a7)+,a1-a2/a5
                moveq   #0,d0
                rts
