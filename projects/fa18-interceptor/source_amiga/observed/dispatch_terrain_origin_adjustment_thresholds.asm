; Byte-exact static threshold cases $C29226-$C29367.
; These jump-table targets compare the maximum component delta in D3 with
; literal thresholds and update control bytes. Meanings remain unassigned.

                org     $C29226

ORIGIN_ADJUSTMENT_MODE    equ     $C457B6
ORIGIN_THRESHOLD_FLAG     equ     $C457B4
ORIGIN_GATE_MODE          equ     $C457AD
ORIGIN_DETAIL_MODE        equ     $C458AE
ORIGIN_AUXILIARY_FLAG     equ     $C458AF
ORIGIN_AUXILIARY_DELTA    equ     $C45C4E
ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56

dispatch_terrain_origin_adjustment_thresholds:
.case_0:
                moveq   #$d,d4
                cmpi.l  #$1c00000,d3
                bgt.w   $C29506
                move.b  #1,ORIGIN_THRESHOLD_FLAG.l
                clr.b   ORIGIN_GATE_MODE.l
                moveq   #$b,d4
                cmpi.l  #$d00000,d3
                bgt.w   $C29506
                move.b  #1,ORIGIN_ADJUSTMENT_MODE.l
                bra.w   $C2940A
.case_1:
                moveq   #$b,d4
                cmpi.l  #$200000,d3
                bgt.w   $C29506
                moveq   #$a,d4
                cmpi.l  #$60000,d3
                bgt.w   $C29506
                move.b  #2,ORIGIN_ADJUSTMENT_MODE.l
                bra.w   $C2940A
.case_2:
                moveq   #9,d4
                cmpi.l  #$60000,d3
                bgt.w   $C2940A
                moveq   #7,d4
                cmpi.l  #$30000,d3
                bgt.w   $C2940A
                move.b  #3,ORIGIN_ADJUSTMENT_MODE.l
                bra.w   $C29488
.case_3:
                moveq   #7,d4
                cmpi.l  #$60000,d3
                bgt.w   $C29488
                moveq   #6,d4
                cmpi.l  #$60000,d3
                bgt.w   $C29488
                moveq   #5,d4
                cmpi.l  #$d000,d3
                bgt.w   $C29488
                moveq   #3,d4
                cmpi.l  #$8000,d3
                bgt.w   $C29488
                move.b  #4,ORIGIN_ADJUSTMENT_MODE.l
                bra.w   $C294AC
.case_4:
                moveq   #2,d4
                cmpi.l  #$2800,d3
                bgt.w   $C29506
                move.b  #5,ORIGIN_ADJUSTMENT_MODE.l
                bra.w   $C294D2
.case_5:
                moveq   #1,d4
                bra.w   $C29506
.case_7:
                tst.b   ORIGIN_AUXILIARY_FLAG.l
                bgt.b   .set_mode_8
                moveq   #$e,d4
                cmpi.l  #$800000,d3
                bgt.w   $C29548
                moveq   #$c,d4
                cmpi.l  #$fff00000,ORIGIN_AUXILIARY_DELTA.l
                bge.b   .set_mode_8
                cmpi.l  #$100000,d3
                bgt.w   $C29548
.set_mode_8:
                move.b  #3,ORIGIN_DETAIL_MODE.l
                move.b  #8,ORIGIN_ADJUSTMENT_MODE.l
                jsr     $C0910C.l
                movem.l d0-d2,ORIGIN_CANDIDATE_TRIPLE.l
                rts
.case_8:
                moveq   #$e,d4
                cmpi.l  #$800000,d3
                bgt.w   $C29548
                moveq   #$c,d4
                cmpi.l  #$100000,d3
                bgt.w   $C29548
                move.b  #4,ORIGIN_DETAIL_MODE.l
                rts
