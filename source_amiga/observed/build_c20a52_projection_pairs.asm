; Byte-exact first pair-building loop $C20AE4-$C20B91.

                org     $C20AE4

PROJECTION_PAIR_WORKSPACE       equ     $C4BF90
PROJECTION_PAIR_FIRST           equ     $C4BF94
PROJECT_CLIP_STAGE              equ     $C246A0

build_c20a52_projection_pairs:
                clr.w   -$7e(a6)
                lea     PROJECTION_PAIR_WORKSPACE.l,a0
                move.l  #4,(a0)+
                clr.w   d4
                clr.w   d5
                clr.w   d6
                bra.b   .build_pair
.advance_pair:
                movem.w -$58(a6),d4-d6
                add.w   -$40(a6),d4
                add.w   -$3e(a6),d5
                add.w   -$3c(a6),d6
.build_pair:
                movem.w d4-d6,-$58(a6)
                lea     PROJECTION_PAIR_FIRST.l,a0
                movem.w (a3),d1-d3
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                add.w   d4,d1
                add.w   d5,d2
                add.w   d6,d3
                move.w  d1,d4
                move.w  d2,d5
                move.w  d3,d6
                add.w   -$46(a6),d4
                add.w   -$44(a6),d5
                add.w   -$42(a6),d6
                movem.w d1-d6,(a0)
                ; ADDA.W #$000C,A0; retain the original immediate encoding.
                dc.w    $D0FC,$000C
                movem.w (a4),d4-d6
                movem.w -$58(a6),d1-d3
                asr.w   #1,d1
                asr.w   #1,d2
                asr.w   #1,d3
                add.w   d1,d4
                add.w   d2,d5
                add.w   d3,d6
                move.w  d4,d1
                move.w  d5,d2
                move.w  d6,d3
                add.w   -$46(a6),d1
                add.w   -$44(a6),d2
                add.w   -$42(a6),d3
                movem.w d1-d6,(a0)
                movem.l a3-a4,-(sp)
                jsr     PROJECT_CLIP_STAGE.l
                or.w    d0,-$7e(a6)
                movem.l (sp)+,a3-a4
                subq.w  #1,-$38(a6)
                bgt.w   .advance_pair
                clr.w   d4
                clr.w   d5
                clr.w   d6
                bra.b   $C20BA4
