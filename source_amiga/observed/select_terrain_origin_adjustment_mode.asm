; Byte-exact static adjustment-mode selector $C291D4-$C29225.
; It compares the candidate triple in C45C56 with the live selector origin in
; C45C3E, retains the greatest absolute component delta in D3, then dispatches
; through the byte-selected table at C28F2C.  The target policy is separate.

                org     $C291D4

ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56
TERRAIN_SELECTOR_ORIGIN   equ     $C45C3E
ORIGIN_ADJUSTMENT_MODE    equ     $C457B6

select_terrain_origin_adjustment_mode:
                movem.l ORIGIN_CANDIDATE_TRIPLE.l,d5-d7
                movem.l TERRAIN_SELECTOR_ORIGIN.l,d0-d2
                sub.l   d0,d5
                move.l  d5,d0
                bge.b   .first_delta_absolute
                neg.l   d0
.first_delta_absolute:
                sub.l   d1,d6
                move.l  d6,d1
                bge.b   .second_delta_absolute
                neg.l   d1
.second_delta_absolute:
                sub.l   d2,d7
                move.l  d7,d2
                bge.b   .third_delta_absolute
                neg.l   d2
.third_delta_absolute:
                cmp.l   d0,d1
                bgt.b   .first_is_not_greatest
                cmp.l   d0,d2
                bgt.b   .first_is_not_greatest_or_second
                move.l  d0,d3
                bra.b   .dispatch_mode
.first_is_not_greatest:
                cmp.l   d1,d2
                ble.b   .second_is_greatest
.first_is_not_greatest_or_second:
                move.l  d2,d3
                bra.b   .dispatch_mode
.second_is_greatest:
                move.l  d1,d3
.dispatch_mode:
                move.b  ORIGIN_ADJUSTMENT_MODE.l,d0
                ext.w   d0
                asl.w   #2,d0
                lea.l   $C28F2C(pc),a0
                movea.l (a0,d0.w),a0
                jmp     (a0)
