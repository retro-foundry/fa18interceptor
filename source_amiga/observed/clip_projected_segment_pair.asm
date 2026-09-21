; Byte-exact clipping continuation $C247C0-$C248B1.
; Tuple-cache continuation is external at $C248B2.

                org     $C247C0

CLIP_POINT_B                    equ     $C4E91A
CLIP_FLAGS                      equ     $C4E874
PROJECTION_ERROR_SLOT           equ     $C4599E
REPORT_PROJECTION_ERROR         equ     $C06C02

clip_projected_segment_pair:
                movem.w d0-d2,-(a7)
                movem.w (a3),d0-d2
                tst.b   $1(a4)
                bne.b   .have_second_endpoint
                movem.w d0-d2,$16(a2)
                addq.b  #1,$1(a4)
                bra.w   $C2487A
.have_second_endpoint:
                movem.w $10(a2),d3-d5
                neg.w   d1
                neg.w   d4
                cmp.w   d2,d1
                bgt.b   .current_after_previous
                cmp.w   d5,d4
                ble.w   .finish_second_side
                bra.b   .clip_crossing
.current_after_previous:
                cmp.w   d5,d4
                bgt.w   .finish_second_side
.clip_crossing:
                neg.w   d1
                neg.w   d4
                movem.w d0-d2,-(a7)
                sub.w   d4,d1
                neg.w   d2
                add.w   d5,d2
                sub.w   d1,d2
                beq.b   .skip_intersection
                add.w   d4,d5
                muls.w  d5,d1
                move.w  d2,d6
                bge.b   .intersection_sign_ready
                neg.w   d6
.intersection_sign_ready:
                asr.w   #1,d6
                divs.w  d2,d1
                swap    d1
                tst.w   d1
                bge.b   .intersection_y_magnitude_ready
                neg.w   d1
.intersection_y_magnitude_ready:
                cmp.w   d1,d6
                bgt.b   .use_intersection_y_fraction
                swap    d1
                tst.w   d1
                bge.b   .round_intersection_y_up
                subq.w  #1,d1
                bra.b   .intersection_y_ready
.round_intersection_y_up:
                addq.w  #1,d1
                bra.b   .intersection_y_ready
.use_intersection_y_fraction:
                swap    d1
.intersection_y_ready:
                add.w   d4,d1
                sub.w   d3,d0
                muls.w  d5,d0
                divs.w  d2,d0
                swap    d0
                tst.w   d0
                bge.b   .intersection_x_magnitude_ready
                neg.w   d0
.intersection_x_magnitude_ready:
                cmp.w   d0,d6
                bgt.b   .use_intersection_x_fraction
                swap    d0
                tst.w   d0
                bge.b   .round_intersection_x_up
                subq.w  #1,d0
                bra.b   .intersection_x_ready
.round_intersection_x_up:
                addq.w  #1,d0
                bra.b   .intersection_x_ready
.use_intersection_x_fraction:
                swap    d0
.intersection_x_ready:
                add.w   d3,d0
                move.w  d1,d2
                neg.w   d2
                movem.w d0-d2,(a3)
                movem.w (a7)+,d0-d2
                movem.w d0-d2,$10(a2)
                bsr.w   $C248B2
                addq.b  #1,$5(a4)
                bra.b   .finish_first_side
.skip_intersection:
                bra.b   .report_error
.finish_second_side:
                neg.w   d1
                movem.w d0-d2,$10(a2)
.finish_first_side:
                neg.w   d1
                cmp.w   d2,d1
                bgt.b   .advance
                neg.w   d1
                movem.w d0-d2,(a3)
                bsr.w   $C248B2
                beq.b   .report_error
                addq.b  #1,$5(a4)
.advance:
                moveq   #1,d0
                movem.w (a7)+,d0-d2
                rts
.report_error:
                move.w  #3,PROJECTION_ERROR_SLOT.l
                jsr     REPORT_PROJECTION_ERROR.l
                movem.w (a7)+,d0-d2
                rts
