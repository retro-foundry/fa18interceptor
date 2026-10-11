; Byte-exact outer clipping loop $C2469E-$C247BF.
; Segment-intersection helper is entered at external $C247C0.

                org     $C2469E

POLYGON_WORKSPACE               equ     $C4BF90
CLIP_POINT_A                    equ     $C4B990
CLIP_POINT_B                    equ     $C4E91A
CLIP_POINT_C                    equ     $C4E910
CLIP_FLAGS                      equ     $C4E874
PROJECTION_ERROR_SLOT           equ     $C4599E
REPORT_PROJECTION_ERROR         equ     $C06C02

clip_projected_polygon_segments:
                nop
                link.w  a6,#-$6
                clr.w   d7
                lea     POLYGON_WORKSPACE.l,a0
                move.w  (a0)+,-$2(a6)
                move.w  (a0)+,d0
                cmpi.w  #3,d0
                blt.b   $C2468C
                move.w  d0,-$4(a6)
                lea     CLIP_POINT_A.l,a1
                lea     CLIP_POINT_B.l,a2
                lea     CLIP_POINT_C.l,a3
                lea     CLIP_FLAGS.l,a4
                clr.l   (a4)
                clr.l   $4(a4)
                moveq   #1,d0
                beq.b   $C24688
.next_point:
                subq.w  #1,-$4(a6)
                blt.w   $C24A94
                movem.w (a0)+,d0-d2
                move.w  -$2(a6),d3
                asl.l   d3,d0
                asl.l   d3,d1
                asl.l   d3,d2
                tst.b   (a4)
                bne.b   .have_previous_point
                movem.w d0-d2,$6(a2)
                addq.b  #1,(a4)
                bra.w   .finish_point
.have_previous_point:
                movem.w (a2),d3-d5
                cmp.w   d2,d1
                bgt.b   .current_after_previous
                cmp.w   d5,d4
                ble.w   .finish_point
                bra.b   .clip_crossing
.current_after_previous:
                cmp.w   d5,d4
                bgt.w   .finish_point
.clip_crossing:
                movem.w d0-d2,-(a7)
                sub.w   d4,d1
                neg.w   d2
                add.w   d5,d2
                add.w   d1,d2
                beq.b   .skip_intersection
                sub.w   d4,d5
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
                movem.w d0-d2,(a3)
                movem.w (a7)+,d0-d2
                movem.w d0-d2,(a2)
                bsr.w   $C247C0
                addq.b  #1,$4(a4)
                bra.b   .store_current_point
.skip_intersection:
                bra.b   .report_clip_error
.finish_point:
                movem.w d0-d2,(a2)
.store_current_point:
                cmp.w   d2,d1
                bgt.b   .advance_point
                movem.w d0-d2,(a3)
                bsr.w   $C247C0
                beq.b   .report_clip_error
                addq.b  #1,$4(a4)
.advance_point:
                moveq   #1,d0
                bra.w   $C246DC
.report_clip_error:
                move.w  #1,PROJECTION_ERROR_SLOT.l
                jsr     REPORT_PROJECTION_ERROR.l
                bra.w   $C246DC
