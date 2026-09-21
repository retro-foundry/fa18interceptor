; Byte-exact $C2E888-$C2E8D9 fourth case block of the C2E758 iterator.
                org $C2E888
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E858
CASE_RESULT_SLOT equ $C4E860
dispatch_fourth_display_record_case:
 cmp.w d5,d4
 blt.w $C2E9DC
 move.w 2(a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EB4C
 beq.b $C2E878
 tst.w CASE_SIGNED_WORD.l
 bge.w $C2E9DC
 move.w d4,d6
 neg.w d6
 cmp.w d5,d6
 blt.w $C2E9DC
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EBC2
 bne.w $C2E9DC
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.w $C2E9F8
