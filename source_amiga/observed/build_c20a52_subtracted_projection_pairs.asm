; Byte-exact alternate pair build in the $C20A52 projection loop,
; $C20B92-$C20C0F.  It submits two six-word workspace records to C246A0.

                org     $C20B92

PROJECTION_PAIR_SECOND          equ     $C4BF94
PROJECT_CLIP_STAGE              equ     $C246A0

build_c20a52_subtracted_projection_pairs:
                movem.w -$58(a6),d4-d6
                add.w   -$40(a6),d4
                add.w   -$3e(a6),d5
                add.w   -$3c(a6),d6
                movem.w d4-d6,-$58(a6)
                lea     PROJECTION_PAIR_SECOND.l,a0
                movem.w -$4c(a6),d1-d3
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                sub.w   d4,d1
                sub.w   d5,d2
                sub.w   d6,d3
                move.w  d1,d4
                move.w  d2,d5
                move.w  d3,d6
                sub.w   -$46(a6),d4
                sub.w   -$44(a6),d5
                sub.w   -$42(a6),d6
                movem.w d1-d6,(a0)
                ; ADDA.W #$000C,A0; retain the original immediate encoding.
                dc.w    $D0FC,$000C
                movem.w -$52(a6),d4-d6
                movem.w -$58(a6),d1-d3
                asr.w   #1,d1
                asr.w   #1,d2
                asr.w   #1,d3
                sub.w   d1,d4
                sub.w   d2,d5
                sub.w   d3,d6
                move.w  d4,d1
                move.w  d5,d2
                move.w  d6,d3
                sub.w   -$46(a6),d1
                sub.w   -$44(a6),d2
                sub.w   -$42(a6),d3
                movem.w d1-d6,(a0)
                jsr     PROJECT_CLIP_STAGE.l
