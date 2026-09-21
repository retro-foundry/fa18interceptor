; Byte-exact $C21490-$C214FF four-point projected record preparation.

                org     $C21490

PROJECTED_RECORD_SELECTOR       equ     $C45954
PROJECTED_RECORD_TABLE          equ     $C48390
PROJECTED_QUAD_WORKSPACE        equ     $C4BF90
PROJECTED_DEPTH                 equ     $C45A78

prepare_four_point_projected_record:
                move.w  (a2)+,PROJECTED_RECORD_SELECTOR.l
                lea     PROJECTED_RECORD_TABLE.l,a3
                adda.w  (a2)+,a3
                cmpi.l  #-$80,PROJECTED_DEPTH.l
                blt.b   .reject
                lea     PROJECTED_QUAD_WORKSPACE.l,a0
                clr.w   (a0)+
                move.w  #4,(a0)+
                movem.w (a3)+,d2-d7
                movem.w d2-d7,(a0)
                ; Preserve ADDA.W immediate; VASM otherwise shortens this to LEA.
                dc.w    $D0FC,$000C             ; adda.w #$C,a0
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movem.w (a3),d2-d4
                move.w  d2,(a0)+
                move.w  d3,(a0)+
                move.w  d4,(a0)+
                sub.w   d5,d2
                sub.w   d6,d3
                sub.w   d7,d4
                movem.w d2-d4,(a0)
                move.w  d4,d7
                and.w   $A(a0),d7
                and.w   $10(a0),d7
                and.w   $16(a0),d7
                blt.b   .reject
                movem.l a1-a2/a5,-(a7)
                jsr     $C2469E.l
                movem.l (a7)+,a1-a2/a5
                rts
.reject:
                moveq   #0,d0
                rts
