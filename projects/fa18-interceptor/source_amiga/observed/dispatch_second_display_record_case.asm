; Byte-exact $C2E7FC-$C2E833 second case block of the C2E758 iterator.
                org $C2E7FC
CASE_SIGNED_WORD equ $C45ACA
dispatch_second_display_record_case:
 move.w d3,d6
 neg.w d6
 cmp.w d5,d6
 blt.w $C2E8DA
 move.w (a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EAD0
 beq.w $C2E9CA
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E834
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E834
 bsr.w $C2EA5A
 beq.w $C2E986
