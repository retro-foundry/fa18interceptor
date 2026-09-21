; Byte-exact $C2EA5A-$C2EACF arithmetic helper called by $C2E758.
; The resulting status is evaluated by the shared tail at $C2EC36.
                org $C2EA5A
adjust_display_record_pair:
 movem.l d0-d6,-(sp)
 movem.w (a1,d1.w),d0-d2
 subq.w #1,d3
 subq.w #1,d4
 move.w d2,d6
 sub.w d5,d6
 sub.w d2,d0
 neg.w d0
 sub.w d5,d3
 add.w d0,d3
 beq.b .reject
 sub.w d1,d4
 neg.w d4
 muls.w d0,d4
 move.w d3,d5
 bge.b .positive_divisor_a
 neg.w d5
.positive_divisor_a:
 asr.w #1,d5
 divs.w d3,d4
 swap d4
 tst.w d4
 bge.b .positive_remainder_a
 neg.w d4
.positive_remainder_a:
 cmp.w d4,d5
 bgt.b .use_quotient_a
 swap d4
 tst.w d4
 bge.b .round_up_a
 subq.w #1,d4
 bra.b .rounded_a
.round_up_a:
 addq.w #1,d4
 bra.b .rounded_a
.use_quotient_a:
 swap d4
.rounded_a:
 sub.w d4,d1
 muls.w d0,d6
 divs.w d3,d6
 swap d6
 tst.w d6
 bge.b .positive_remainder_b
 neg.w d6
.positive_remainder_b:
 cmp.w d6,d5
 bgt.b .use_quotient_b
 swap d6
 tst.w d6
 bge.b .round_up_b
 subq.w #1,d6
 bra.b .rounded_b
.round_up_b:
 addq.w #1,d6
 bra.b .rounded_b
.use_quotient_b:
 swap d6
.rounded_b:
 sub.w d6,d2
 move.w d2,d0
 bra.w $C2EC36
.reject:
 bra.w $C2EC58
