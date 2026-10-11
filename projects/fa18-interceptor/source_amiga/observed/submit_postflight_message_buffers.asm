; Byte-exact postflight message-buffer submission $C325A6-$C32678.
; The buffer/style dataflow is proven; final glyph placement remains unassigned.

                org     $C325A6

POSTFLIGHT_PRIMARY_TEXT         equ     $C4580A
POSTFLIGHT_TEXT_STYLE           equ     $C45862
POSTFLIGHT_RENDER_LANE          equ     $C45986
POSTFLIGHT_RENDER_OFFSET        equ     $C45918
POSTFLIGHT_MODE_FLAG            equ     $C45785
POSTFLIGHT_MODE_LATCH           equ     $C45793
POSTFLIGHT_REDRAW_DELAY         equ     $C45861
POSTFLIGHT_RENDER_MODE_A        equ     $C3278C
POSTFLIGHT_RENDER_MODE_B        equ     $C32794

submit_postflight_message_buffers:
                lea.l   POSTFLIGHT_PRIMARY_TEXT.l,a2
                ; LEA $C31998(PC),A1; preserve PC-relative encoding.
                dc.w    $43FA,$F3EA
                lea.l   $1E0C.w,a4
                lea.l   $C.w,a5
                moveq   #$19,d0
                move.w  #$FCA,d6
                swap    d6
                move.w  POSTFLIGHT_RENDER_LANE.l,d6
                move.l  POSTFLIGHT_RENDER_OFFSET.l,d7
                movem.l d0/d6-d7/a1-a2/a4-a5,-(a7)
                move.b  POSTFLIGHT_TEXT_STYLE.l,d3
                rol.b   #2,d3
                andi.b  #3,d3
                beq.b   .style_zero
                subq.b  #1,d3
                beq.b   .style_one
                move.w  #$C,d5
                bsr.w   .submit_by_mode
                move.w  #4,d5
                move.w  #0,d3
                bra.b   .submit_second_pass
.style_zero:
                move.w  #0,d5
                bsr.w   .submit_by_mode
                move.w  #$C,d5
                move.w  #4,d3
                bra.b   .submit_second_pass
.style_one:
                move.w  #4,d5
                bsr.w   .submit_by_mode
                move.w  #$C,d5
                move.w  #0,d3
.submit_second_pass:
                movem.l (a7)+,d0/d6-d7/a1-a2/a4-a5
                tst.b   POSTFLIGHT_MODE_FLAG.l
                beq.b   .normal_mode
                tst.b   POSTFLIGHT_MODE_LATCH.l
                beq.b   .return
                lea.l   $1B14.w,a4
                lea.l   $C.w,a5
                move.w  #$C,d5
                move.w  #$F3A,d6
                bra.w   POSTFLIGHT_RENDER_MODE_A
.normal_mode:
                tst.b   POSTFLIGHT_REDRAW_DELAY.l
                blt.b   .return
                subq.b  #1,POSTFLIGHT_REDRAW_DELAY.l
                swap    d6
                move.w  #$F0A,d6
                swap    d6
                movem.l d0/d3/d6-d7/a1-a2/a4-a5,-(a7)
                bsr.w   .submit_by_mode
                movem.l (a7)+,d0/d3/d6-d7/a1-a2/a4-a5
                move.w  d3,d5
.submit_by_mode:
                tst.b   POSTFLIGHT_MODE_FLAG.l
                beq.w   POSTFLIGHT_RENDER_MODE_B
                tst.b   POSTFLIGHT_MODE_LATCH.l
                beq.b   .return
                bra.w   POSTFLIGHT_RENDER_MODE_B
.return:
                rts

