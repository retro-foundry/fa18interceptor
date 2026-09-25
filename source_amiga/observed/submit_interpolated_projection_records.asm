; Byte-exact observed interpolated projection-record submission $C20CA0-$C20D67.
; Builds two six-word transformed tuples in the C4BF94 workspace per loop and
; calls the established projection renderer.  Object/record identity is open.

                org     $C20CA0

PROJECTION_COUNT_MARKER        equ     $C4BF90
PROJECTION_WORKSPACE           equ     $C4BF94
RENDER_PROJECTION_RECORD       equ     $C246A0

submit_interpolated_projection_records:
                lea     PROJECTION_COUNT_MARKER.l,a0
                move.l  #4,(a0)+
                bra.s   .load_inner_count
.advance_outer_source:
                addq.w  #6,a3
.load_inner_count:
                move.w  (a2)+,-$3a(a6)
                clr.w   d4
                clr.w   d5
                clr.w   d6
                bra.s   .store_accumulator
.advance_accumulator:
                movem.w -$58(a6),d4-d6
                add.w   -$40(a6),d4
                add.w   -$3e(a6),d5
                add.w   -$3c(a6),d6
.store_accumulator:
                movem.w d4-d6,-$58(a6)
                lea     PROJECTION_WORKSPACE.l,a0
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
                ; VASM shortens this to LEA; retain the original immediate ADDA.W.
                dc.w    $D0FC,$000C             ; adda.w #$000C,a0
                movem.w (a3),d4-d6
                add.w   -$52(a6),d4
                add.w   -$50(a6),d5
                add.w   -$4e(a6),d6
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
                movem.l a2-a3,-(sp)
                jsr     RENDER_PROJECTION_RECORD.l
                or.w    d0,-$7e(a6)
                movem.l (sp)+,a2-a3
                subq.w  #1,-$3a(a6)
                bgt.w   .advance_accumulator
                subq.w  #1,-$38(a6)
                bgt.w   .advance_outer_source
                movem.l (sp)+,a1/a5
                move.w  -$7e(a6),d0
                rts
