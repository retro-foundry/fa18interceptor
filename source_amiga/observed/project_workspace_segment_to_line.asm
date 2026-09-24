; Byte-exact static projection primitive $C2ED6C-$C2EE42.
; Reads two triples from C4C592, rejects either outside its signed depth bounds,
; projects/clamps two screen pairs, and submits the segment through C2FA7E.
; Static structure is clear; no independent entry trace is claimed.
                org     $C2ED6C
PROJECT_SEGMENT_INPUT equ     $C4C592
EMIT_PROJECTED_LINE   equ     $C2FA7E
project_workspace_segment_to_line_reject:
                moveq   #0,d0
                rts
project_workspace_segment_to_line:
                lea.l   PROJECT_SEGMENT_INPUT.l,a1
                move.w  (a1)+,d0
                move.w  (a1)+,d1
                move.w  (a1)+,d2
                ble.b   project_workspace_segment_to_line_reject
                cmp.w   d2,d0
                bgt.b   project_workspace_segment_to_line_reject
                move.w  d0,d5
                neg.w   d5
                cmp.w   d2,d5
                bgt.b   project_workspace_segment_to_line_reject
                cmp.w   d2,d1
                bgt.b   project_workspace_segment_to_line_reject
                move.w  d1,d5
                neg.w   d5
                cmp.w   d2,d5
                bgt.b   project_workspace_segment_to_line_reject
                muls.w  #$a0,d0
                divs.w  d2,d0
                addi.w  #$a0,d0
                bge.b   .first_x_nonnegative
                clr.w   d0
                bra.b   .first_x_done
.first_x_nonnegative:
                cmpi.w  #$140,d0
                blt.b   .first_x_done
                move.w  #$13f,d0
.first_x_done:
                muls.w  #$5a,d1
                divs.w  d2,d1
                addi.w  #$5a,d1
                bge.b   .first_y_nonnegative
                clr.w   d1
                bra.b   .first_y_done
.first_y_nonnegative:
                cmpi.w  #$b4,d1
                blt.b   .first_y_done
                move.w  #$b3,d1
.first_y_done:
                subi.w  #$13f,d0
                neg.w   d0
                subi.w  #$b3,d1
                neg.w   d1
                move.w  (a1)+,d2
                move.w  (a1)+,d3
                move.w  (a1)+,d4
                ble.b   .second_reject
                cmp.w   d4,d2
                bgt.b   .second_reject
                move.w  d2,d5
                neg.w   d5
                cmp.w   d4,d5
                bgt.b   .second_reject
                cmp.w   d4,d3
                bgt.b   .second_reject
                move.w  d3,d5
                neg.w   d5
                cmp.w   d4,d5
                bgt.b   .second_reject
                muls.w  #$a0,d2
                divs.w  d4,d2
                addi.w  #$a0,d2
                bge.b   .second_x_nonnegative
                clr.w   d2
                bra.b   .second_x_done
.second_x_nonnegative:
                cmpi.w  #$140,d2
                blt.b   .second_x_done
                move.w  #$13f,d2
.second_x_done:
                muls.w  #$5a,d3
                divs.w  d4,d3
                addi.w  #$5a,d3
                bge.b   .second_y_nonnegative
                clr.w   d3
                bra.b   .second_y_done
.second_y_nonnegative:
                cmpi.w  #$b4,d3
                blt.b   .second_y_done
                move.w  #$b3,d3
.second_y_done:
                subi.w  #$13f,d2
                neg.w   d2
                subi.w  #$b3,d3
                neg.w   d3
                jsr     EMIT_PROJECTED_LINE.l
                moveq   #1,d0
                rts
.second_reject:
                moveq   #0,d0
                rts
