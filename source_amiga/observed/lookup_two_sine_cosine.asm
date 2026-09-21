; Byte-exact reconstruction of $C2E5F6-$C2E6D9 (Hunk 32 +$1DA).
; Input:  D0.w and D2.w are angles in native game units.
; Output: (D0.w,D1.w) and (D2.w,D3.w) are sine/cosine pairs.
; Clobbers: D6.w, D7.w, A0.

                org     $C2E5F6

TRIG_TABLE_QUADRANT_BYTES equ $0708
TRIG_TABLE_HALF_TURN      equ $0E10
TRIG_TABLE_THREE_QUARTERS equ $1518
TRIG_TABLE_FULL_TURN      equ $1C20
trig_quadrant_word_table  equ $C3E5E8

lookup_two_sine_cosine:
                add.w   d0,d0
                add.w   d2,d2
                lea     trig_quadrant_word_table,a0

                move.w  d0,d7
                cmpi.w  #TRIG_TABLE_QUADRANT_BYTES,d0
                bge.s   .first_second_quadrant
.first_first_quadrant:
                move.w  (a0,d0.w),d0
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d1
                bra.s   .first_done
.first_second_quadrant:
                cmpi.w  #TRIG_TABLE_HALF_TURN,d0
                bge.s   .first_third_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d0,d6
                move.w  (a0,d6.w),d0
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d1
                neg.w   d1
                bra.s   .first_done
.first_third_quadrant:
                cmpi.w  #TRIG_TABLE_THREE_QUARTERS,d0
                bge.s   .first_fourth_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d6,d0
                move.w  (a0,d0.w),d0
                neg.w   d0
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d1
                neg.w   d1
                bra.s   .first_done
.first_fourth_quadrant:
                move.w  #TRIG_TABLE_FULL_TURN,d6
                sub.w   d0,d6
                move.w  (a0,d6.w),d0
                neg.w   d0
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d1

.first_done:
                move.w  d2,d7
                cmpi.w  #TRIG_TABLE_QUADRANT_BYTES,d2
                bge.s   .second_second_quadrant
.second_first_quadrant:
                move.w  (a0,d2.w),d2
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d3
                bra.s   .done
.second_second_quadrant:
                cmpi.w  #TRIG_TABLE_HALF_TURN,d2
                bge.s   .second_third_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d2,d6
                move.w  (a0,d6.w),d2
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d3
                neg.w   d3
                bra.s   .done
.second_third_quadrant:
                cmpi.w  #TRIG_TABLE_THREE_QUARTERS,d2
                bge.s   .second_fourth_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d6,d2
                move.w  (a0,d2.w),d2
                neg.w   d2
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d3
                neg.w   d3
                bra.s   .done
.second_fourth_quadrant:
                move.w  #TRIG_TABLE_FULL_TURN,d6
                sub.w   d2,d6
                move.w  (a0,d6.w),d2
                neg.w   d2
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d3
.done:
                rts
