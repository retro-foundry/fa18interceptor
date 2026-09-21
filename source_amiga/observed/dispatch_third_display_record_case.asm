; Byte-exact $C2E834-$C2E887 third case block of the C2E758 iterator.
                org $C2E834
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E854
CASE_RESULT_SLOT equ $C4E85C
dispatch_third_display_record_case:
 tst.w d4
 bge.b $C2E888
 move.w d4,d6
 neg.w d6
 cmp.w d5,d6
 blt.w $C2E9DC
 move.w 2(a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EBC2
 beq.b $C2E8CA
 tst.w CASE_SIGNED_WORD.l
 bge.w $C2E9DC
 cmp.w d5,d4
 blt.w $C2E9DC
 neg.w d2
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EB4C
 bne.w $C2E9DC
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.w $C2E9F8
