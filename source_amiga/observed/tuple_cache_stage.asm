; Byte-exact observed-entry slice $C248B2-$C24981 (Hunk 19 +$22A).
; Inputs: A2/A3/A4 point to caller-owned tuple/cache state.
; Branches to $C24982 leave this slice.

                org     $C248B2

TUPLE_STAGE_REJECT             equ $C24982
BRANCH_TO_NEXT_INSTRUCTION     equ $6000

tuple_cache_stage:
                movem.w d0-d2,-(a7)
                movem.w (a3),d0-d2
                tst.b   2(a4)
                bne.s   .have_cached_tuple
                movem.w d0-d2,$26(a2)
                addq.b  #1,2(a4)
                bra.w   .store_current_tuple

.have_cached_tuple:
                movem.w $20(a2),d3-d5
                move.w  d2,d6
                cmp.w   d2,d0
                bgt.s   .current_first_greater
                cmp.w   d5,d3
                ble.w   .store_current_tuple
                bra.s   .interpolate_tuple
.current_first_greater:
                cmp.w   d5,d3
                bgt.w   .store_current_tuple

.interpolate_tuple:
                movem.w d0-d2,-(a7)
                sub.w   d3,d0
                neg.w   d2
                add.w   d5,d2
                add.w   d0,d2
                beq.s   .reject
                sub.w   d3,d5
                muls.w  d5,d0
                move.w  d2,d6
                bge.s   .positive_divisor_x
                neg.w   d6
.positive_divisor_x:
                asr.w   #1,d6
                divs.w  d2,d0
                swap    d0
                tst.w   d0
                bge.s   .x_remainder_positive
                neg.w   d0
.x_remainder_positive:
                cmp.w   d0,d6
                bgt.s   .x_swap_quotient
                swap    d0
                tst.w   d0
                bge.s   .increment_x_quotient
                subq.w  #1,d0
                bra.s   .x_add_base
.increment_x_quotient:
                addq.w  #1,d0
                bra.s   .x_add_base
.x_swap_quotient:
                swap    d0
.x_add_base:
                add.w   d3,d0

                sub.w   d4,d1
                muls.w  d5,d1
                divs.w  d2,d1
                swap    d1
                tst.w   d1
                bge.s   .y_remainder_positive
                neg.w   d1
.y_remainder_positive:
                cmp.w   d1,d6
                bgt.s   .y_swap_quotient
                swap    d1
                tst.w   d1
                bge.s   .increment_y_quotient
                subq.w  #1,d1
                bra.s   .y_add_base
.increment_y_quotient:
                addq.w  #1,d1
                bra.s   .y_add_base
.y_swap_quotient:
                swap    d1
.y_add_base:
                add.w   d4,d1
                move.w  d0,d2
                movem.w d0-d2,(a3)
                movem.w (a7)+,d0-d2
                movem.w d0-d2,$20(a2)
                bsr.w   update_tuple_cache_status
                addq.b  #1,6(a4)
                bra.s   .compare_tuple_order

.reject:
                bra.s   TUPLE_STAGE_REJECT
.store_current_tuple:
                movem.w d0-d2,$20(a2)
.compare_tuple_order:
                cmp.w   d2,d0
                bgt.s   .return_success
                movem.w d0-d2,(a3)
                bsr.w   update_tuple_cache_status
                beq.s   TUPLE_STAGE_REJECT
                addq.b  #1,6(a4)
.return_success:
                moveq   #1,d0
                movem.w (a7)+,d0-d2
                rts

update_tuple_cache_status equ $C24996
