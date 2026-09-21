; Byte-exact polygon projection/output tail $C24CFE-$C24DA7.

                org     $C24CFE

CLIPPED_TUPLE_LIST             equ     $C4B990
PROJECTED_TUPLE_LIST           equ     $C4B390
SUBMIT_TUPLE_LIST              equ     $C2FF48
SUBMITTED_TUPLE_LIST_COUNT     equ     $C46182
PROJECTION_FAILURE_COUNT       equ     $C458EC
PROJECTION_ERROR_SLOT          equ     $C4599E
REPORT_PROJECTION_ERROR        equ     $C06C02

project_clamp_and_submit_polygon_tuple_list:
                tst.w   d7
                beq.w   .return_zero
                cmpi.w  #2,d7
                ble.b   $C24D28
                lea     CLIPPED_TUPLE_LIST.l,a1
                lea     PROJECTED_TUPLE_LIST.l,a0
                move.w  #$13F,d0
                move.w  #$B3,d1
                move.w  d7,(a0)+
                subq.w  #1,d7
.project_next_tuple:
                movem.w (a1)+,d3-d5
                tst.w   d5
                ble.w   $C24DA0
                muls.w  #$A0,d3
                divs.w  d5,d3
                addi.w  #$A0,d3
                blt.b   .clamp_x_low
                cmpi.w  #$140,d3
                bge.b   .clamp_x_high
.x_ready:
                muls.w  #$5A,d4
                divs.w  d5,d4
                addi.w  #$5A,d4
                blt.b   .clamp_y_low
                cmpi.w  #$B4,d4
                bge.b   .clamp_y_high
.y_ready:
                move.w  d0,d2
                move.w  d1,d5
                sub.w   d3,d2
                sub.w   d4,d5
                move.w  d2,(a0)+
                move.w  d5,(a0)+
                dbra    d7,.project_next_tuple
                jsr     SUBMIT_TUPLE_LIST.l
                addq.w  #1,SUBMITTED_TUPLE_LIST_COUNT.l
                unlk    a6
                moveq   #1,d0
                rts
.return_zero:
                unlk    a6
                moveq   #0,d0
                rts
.clamp_x_low:
                clr.w   d3
                bra.b   .x_ready
.clamp_y_low:
                clr.w   d4
                bra.b   .y_ready
.clamp_x_high:
                move.w  #$13F,d3
                bra.b   .x_ready
.clamp_y_high:
                move.w  #$B3,d4
                bra.b   .y_ready
.common_projection_failure:
                addq.w  #1,PROJECTION_FAILURE_COUNT.l
                move.w  #$A,PROJECTION_ERROR_SLOT.l
                jsr     REPORT_PROJECTION_ERROR.l
                clr.w   d7
                unlk    a6
                moveq   #0,d0
                rts
