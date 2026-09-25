; Byte-exact observed marker-line-list entry $C2B93E-$C2B951.
; On a nonnegative incoming condition it selects the three-record signed-offset
; list at $C2B91E and joins the common clipped line iterator at $C2BAA8.
; The symbol's game identity is deliberately not assigned here.

                org     $C2B93E

MARKER_LINE_COUNT               equ     $0003
MARKER_LINE_LIST                equ     $C2B91E
COMMON_CLIPPED_LINE_ITERATOR    equ     $C2BAA8
MARKER_LINE_STATUS              equ     $C45954

submit_three_record_marker_line_list:
                blt.s   .return
                move.w  #MARKER_LINE_COUNT,MARKER_LINE_STATUS.l
                lea     MARKER_LINE_LIST(pc),a1
                bra.w   COMMON_CLIPPED_LINE_ITERATOR
.return:
                rts
