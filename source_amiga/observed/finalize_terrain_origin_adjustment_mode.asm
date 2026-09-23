; Byte-exact adjustment-mode exit $C29506-$C29547.
; This low-magnitude state transition either returns or leaves D3-D7 prepared
; for the common adjustment/publish tail at C29548.

                org     $C29506

ORIGIN_ADJUSTMENT_MODE    equ     $C457B6
ORIGIN_THRESHOLD_FLAG     equ     $C457B4
ORIGIN_DETAIL_MODE        equ     $C458AE
ORIGIN_DETAIL_COUNTER     equ     $C458A5
ORIGIN_STATUS_WORD        equ     $C458C6

finalize_terrain_origin_adjustment_mode:
                cmpi.l  #$240,d3
                bgt.b   $C29548
                cmpi.b  #6,ORIGIN_DETAIL_MODE.l
                blt.b   .return
                move.b  #6,ORIGIN_ADJUSTMENT_MODE.l
                move.b  #5,ORIGIN_DETAIL_COUNTER.l
                move.b  #0,ORIGIN_THRESHOLD_FLAG.l
                subq.b  #1,ORIGIN_DETAIL_COUNTER.l
                bgt.b   .return
                ori.w   #2,ORIGIN_STATUS_WORD.l
                clr.b   ORIGIN_DETAIL_MODE.l
.return:
                rts
