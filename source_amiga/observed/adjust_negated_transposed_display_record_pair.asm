; Byte-exact $C2EBC2-$C2EC35 static sibling helper reached from $C2E758.
; It falls through to the shared $C2EC36 bound-classification tail.
                org $C2EBC2
adjust_negated_transposed_display_record_pair:
 movem.l d0-d6,-(sp)
 movem.w (a1,d1.w),d0-d2
 subq.w #1,d3
 subq.w #1,d4
 neg.w d1
 neg.w d4
 move.w d2,d6
 sub.w d5,d6
 sub.w d2,d1
 neg.w d1
 sub.w d5,d4
 add.w d1,d4
 beq.b $C2EC58
 sub.w d0,d3
 neg.w d3
 muls.w d1,d3
 move.w d4,d5
 bge.b .positive_divisor_a
 neg.w d5
.positive_divisor_a:
 asr.w #1,d5
 divs.w d4,d3
 swap d3
 tst.w d3
 bge.b .positive_remainder_a
 neg.w d3
.positive_remainder_a:
 cmp.w d3,d5
 bgt.b .use_quotient_a
 swap d3
 tst.w d3
 bge.b .round_up_a
 subq.w #1,d3
 bra.b .rounded_a
.round_up_a:
 addq.w #1,d3
 bra.b .rounded_a
.use_quotient_a:
 swap d3
.rounded_a:
 sub.w d3,d0
 muls.w d1,d6
 divs.w d4,d6
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
 move.w d2,d1
 neg.w d1
