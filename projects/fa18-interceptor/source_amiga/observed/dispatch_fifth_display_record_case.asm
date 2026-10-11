; Byte-exact $C2E8DA-$C2E915 fifth case block of the C2E758 iterator.
                org $C2E8DA
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E858
CASE_RESULT_SLOT equ $C4E860
dispatch_fifth_display_record_case:
 cmp.w d5,d4
 blt.b $C2E916
 move.w 2(a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EB4C
 beq.b $C2E878
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E94C
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E94C
 bsr.w $C2EBC2
 bne.b $C2E94C
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.w $C2E9F8
