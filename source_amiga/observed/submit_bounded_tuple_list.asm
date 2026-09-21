; Byte-exact observed-entry slice $C301F6-$C302C3 (Hunk 36 +$DA6).
; Inputs: D2/D4 and D3/D5 form caller bounds.  The list and its record meaning
; remain unknown.  Branches to $C302C4 leave this slice.

                org     $C301F6

DISPLAY_BOUND_Y                equ $C45984
TUPLE_LIST_BASE                equ $C4B390
LINE_EMITTER_SCRATCH           equ $C456E6
LINE_EMITTER_MODE_FLAG         equ $C457A2
SUBMIT_NEAR_LINE               equ $C2F66E
BLITTER_DRAW_LINE              equ $C2FA7E
OUTSIDE_OBSERVED_ENTRY_SLICE   equ $C302C4
LIST_FAR_VERTICAL_EXIT         equ $C302DE
LIST_FAR_HORIZONTAL_EXIT       equ $C302EC
LIST_NEAR_EXIT                 equ $C302DA

submit_bounded_tuple_list:
                movea.w DISPLAY_BOUND_Y.l,a4
                lea     TUPLE_LIST_BASE.l,a0
                move.w  (a0)+,d6
                subq.w  #3,d6
                movem.w (a0)+,d0-d5

                cmp.w   d0,d2
                bge.s   .first_x_min_ready
                exg.l   d0,d2
.first_x_min_ready:
                cmp.w   d0,d4
                bge.s   .first_x_bounds_ready
                move.w  d4,d0
                bra.s   .first_y_min_ready
.first_x_bounds_ready:
                cmp.w   d2,d4
                ble.s   .first_y_min_ready
                move.w  d4,d2
.first_y_min_ready:
                cmp.w   d1,d3
                bge.s   .first_y_max_ready
                exg.l   d1,d3
.first_y_max_ready:
                cmp.w   d1,d5
                bge.s   .first_bounds_ready
                move.w  d5,d1
                bra.s   .reduce_list
.first_bounds_ready:
                cmp.w   d3,d5
                ble.s   .reduce_list
                move.w  d5,d3
.reduce_list:
                subq.w  #1,d6
                blt.s   .test_vertical_bound
                move.w  (a0)+,d4
                move.w  (a0)+,d5
                cmp.w   d0,d4
                bgt.s   .replace_x_min
                move.w  d4,d0
                bra.s   .reduce_y
.replace_x_min:
                cmp.w   d4,d2
                bge.s   .reduce_y
                move.w  d4,d2
.reduce_y:
                cmp.w   d1,d5
                bgt.s   .replace_y_min
                move.w  d5,d1
                bra.s   .reduce_list
.replace_y_min:
                cmp.w   d5,d3
                bge.s   .reduce_list
                move.w  d5,d3
                bra.s   .reduce_list

.return_success:
                moveq   #1,d0
                rts

.test_vertical_bound:
                cmp.w   a4,d1
                bgt.s   .return_success
                move.w  d3,d7
                sub.w   d1,d7
                bge.s   .vertical_extent_positive
                neg.w   d7
.vertical_extent_positive:
                cmpi.w  #2,d7
                bgt.s   LIST_FAR_VERTICAL_EXIT
                cmpi.w  #1,d7
                ble.s   .test_horizontal_extent
                move.w  d2,d6
                sub.w   d0,d6
                bge.s   .horizontal_extent_positive_far
                neg.w   d6
.horizontal_extent_positive_far:
                cmpi.w  #2,d6
                bgt.s   LIST_FAR_HORIZONTAL_EXIT
                addq.w  #1,d1
                cmp.w   a4,d1
                bgt.s   LIST_NEAR_EXIT
                move.w  d2,d0
                bsr.w   SUBMIT_NEAR_LINE
                bra.s   LIST_NEAR_EXIT

.test_horizontal_extent:
                move.w  d2,d6
                sub.w   d0,d6
                bge.s   .horizontal_extent_positive
                neg.w   d6
.horizontal_extent_positive:
                cmpi.w  #1,d6
                ble.s   OUTSIDE_OBSERVED_ENTRY_SLICE
                move.l  LINE_EMITTER_SCRATCH.l,-(a7)
                tst.b   LINE_EMITTER_MODE_FLAG.l
                bne.s   .emit_line
                move.l  #$000FFFFF,LINE_EMITTER_SCRATCH.l
.emit_line:
                bsr.w   BLITTER_DRAW_LINE
                move.l  (a7)+,LINE_EMITTER_SCRATCH.l
                moveq   #1,d0
                rts
