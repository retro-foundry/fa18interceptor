; Byte-exact reconstruction of $C2E6DA-$C2E74F (Hunk 32 +$BE).
; Input:  D4.w is an angle in the game's native units.
; Output: D4.w = sine component, D5.w = cosine component.
; Clobbers: D6.w, D7.w, A0.
;
; The table's native index is a byte offset.  These doubled-angle constants
; describe table symmetry, not a documented public angle unit.

                org     $C2E6DA

TRIG_TABLE_QUADRANT_BYTES equ $0708
TRIG_TABLE_HALF_TURN      equ $0E10
TRIG_TABLE_THREE_QUARTERS equ $1518
TRIG_TABLE_FULL_TURN      equ $1C20

trig_quadrant_word_table  equ $C3E5E8

lookup_sine_cosine:
                add.w   d4,d4
                lea     trig_quadrant_word_table,a0
                move.w  d4,d7
                cmpi.w  #TRIG_TABLE_QUADRANT_BYTES,d4
                bge.s   .second_quadrant

.first_quadrant:
                move.w  (a0,d4.w),d4
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d5
                bra.s   .done

.second_quadrant:
                cmpi.w  #TRIG_TABLE_HALF_TURN,d4
                bge.s   .third_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d4,d6
                move.w  (a0,d6.w),d4
                move.w  #TRIG_TABLE_QUADRANT_BYTES,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d5
                neg.w   d5
                bra.s   .done

.third_quadrant:
                cmpi.w  #TRIG_TABLE_THREE_QUARTERS,d4
                bge.s   .fourth_quadrant
                move.w  #TRIG_TABLE_HALF_TURN,d6
                sub.w   d6,d4
                move.w  (a0,d4.w),d4
                neg.w   d4
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d7,d6
                move.w  (a0,d6.w),d5
                neg.w   d5
                bra.s   .done

.fourth_quadrant:
                move.w  #TRIG_TABLE_FULL_TURN,d6
                sub.w   d4,d6
                move.w  (a0,d6.w),d4
                neg.w   d4
                move.w  #TRIG_TABLE_THREE_QUARTERS,d6
                sub.w   d6,d7
                move.w  (a0,d7.w),d5

.done:
                rts
