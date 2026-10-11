; Byte-exact $C2E916-$C2E94B sixth case block of the C2E758 iterator.
                org $C2E916
CASE_SIGNED_WORD equ $C45ACA
dispatch_sixth_display_record_case:
 move.w d4,d6
 neg.w d6
 cmp.w d5,d6
 blt.w $C2E9D8
 move.w 2(a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EBC2
 beq.b $C2E8CA
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E94C
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E94C
 bsr.w $C2EB4C
 beq.w $C2E878
