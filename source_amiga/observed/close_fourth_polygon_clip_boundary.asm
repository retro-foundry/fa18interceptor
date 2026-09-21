; Byte-exact fourth polygon-boundary closure $C24C5E-$C24CFD.

                org     $C24C5E

PROJECTION_ERROR_SLOT           equ     $C4599E
REPORT_PROJECTION_ERROR         equ     $C06C02

close_fourth_polygon_clip_boundary:
                tst.b   $7(a4)
                beq.w   $C24CFE
                movem.w $30(a2),d0-d5
                neg.w   d0
                neg.w   d3
                cmp.w   d2,d0
                bgt.b   .current_after_previous
                cmp.w   d5,d3
                ble.w   $C24CFE
                bra.b   .interpolate
.report_error:
                move.w  #9,PROJECTION_ERROR_SLOT.l
                jsr     REPORT_PROJECTION_ERROR.l
                bra.w   $C24D8C
.current_after_previous:
                cmp.w   d5,d3
                bgt.w   $C24CFE
.interpolate:
                neg.w   d0
                neg.w   d3
                sub.w   d3,d0
                neg.w   d2
                add.w   d5,d2
                sub.w   d0,d2
                beq.b   .report_error
                add.w   d3,d5
                muls.w  d5,d0
                move.w  d2,d6
                bge.b   .divisor_ready
                neg.w   d6
.divisor_ready:
                asr.w   #1,d6
                divs.w  d2,d0
                swap    d0
                tst.w   d0
                bge.b   .x_magnitude_ready
                neg.w   d0
.x_magnitude_ready:
                cmp.w   d0,d6
                bgt.b   .use_x_fraction
                swap    d0
                tst.w   d0
                bge.b   .round_x_up
                subq.w  #1,d0
                bra.b   .x_ready
.round_x_up:
                addq.w  #1,d0
                bra.b   .x_ready
.use_x_fraction:
                swap    d0
.x_ready:
                add.w   d3,d0
                sub.w   d4,d1
                muls.w  d5,d1
                divs.w  d2,d1
                swap    d1
                tst.w   d1
                bge.b   .y_magnitude_ready
                neg.w   d1
.y_magnitude_ready:
                cmp.w   d1,d6
                bgt.b   .use_y_fraction
                swap    d1
                tst.w   d1
                bge.b   .round_y_up
                subq.w  #1,d1
                bra.b   .y_ready
.round_y_up:
                addq.w  #1,d1
                bra.b   .y_ready
.use_y_fraction:
                swap    d1
.y_ready:
                add.w   d4,d1
                move.w  d0,d2
                neg.w   d2
                move.w  d0,(a1)+
                move.w  d1,(a1)+
                move.w  d2,(a1)+
                addq.w  #1,d7
