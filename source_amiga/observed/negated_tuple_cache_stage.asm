; Byte-exact observed-entry slice $C24996-$C24A93 (Hunk 19 +$30E).
; Inputs: A1 output cursor; A2/A3/A4 caller-owned tuple/cache state; D7 count.
; Outputs: may append a three-word tuple through A1, increment D7 and cache counters.
; The status/error helper at $C06C02 is outside this slice.

                org     $C24996

TUPLE_STAGE_ERROR_SLOT         equ $C4599E
REPORT_TUPLE_STAGE_ERROR       equ $C06C02
NEGATED_TUPLE_DENOMINATOR_ERROR_CODE equ 5

negated_tuple_cache_stage:
                movem.w d0-d2,-(a7)
                movem.w (a3),d0-d2
                tst.b   3(a4)
                bne.s   .have_cached_tuple
                movem.w d0-d2,$36(a2)
                addq.b  #1,3(a4)
                bra.w   .store_cached_tuple

.have_cached_tuple:
                movem.w $30(a2),d3-d5
                neg.w   d0
                neg.w   d3
                cmp.w   d2,d0
                bgt.s   .current_first_greater
                cmp.w   d5,d3
                ble.w   .store_negated_tuple
                bra.s   .interpolate_tuple

.report_denominator_error:
                bra.w   .report_denominator_error_body

.current_first_greater:
                cmp.w   d5,d3
                bgt.w   .store_negated_tuple

.interpolate_tuple:
                neg.w   d0
                neg.w   d3
                movem.w d0-d2,-(a7)
                sub.w   d3,d0
                neg.w   d2
                add.w   d5,d2
                sub.w   d0,d2
                beq.s   .report_denominator_error
                add.w   d3,d5
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
                move.w  d2,d6
                bge.s   .positive_divisor_y
                neg.w   d6
.positive_divisor_y:
                asr.w   #1,d6
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
                neg.w   d2
                movem.w d0-d2,(a3)
                move.l  (a3),(a1)+
                move.w  4(a3),(a1)+
                addq.w  #1,d7
                movem.w (a7)+,d0-d2
                movem.w d0-d2,$30(a2)
                addq.b  #1,7(a4)
                bra.s   .store_or_emit_tuple

.store_negated_tuple:
                neg.w   d0
.store_cached_tuple:
                movem.w d0-d2,$30(a2)
.store_or_emit_tuple:
                neg.w   d0
                cmp.w   d2,d0
                bgt.s   .return_success
                neg.w   d0
                move.w  d0,(a1)+
                move.w  d1,(a1)+
                move.w  d2,(a1)+
                addq.w  #1,d7
                addq.b  #1,7(a4)
.return_success:
                moveq   #1,d0
                movem.w (a7)+,d0-d2
                rts


.report_denominator_error_body:
                move.w  #NEGATED_TUPLE_DENOMINATOR_ERROR_CODE,TUPLE_STAGE_ERROR_SLOT.l
                jsr     REPORT_TUPLE_STAGE_ERROR.l
                movem.w (a7)+,d0-d2
                rts
