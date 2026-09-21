; Byte-exact $C2E994-$C2E9D7 final D3-signed case block of C2E758.
                org $C2E994
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E85A
CASE_RESULT_SLOT equ $C4E862
dispatch_eighth_display_record_case:
 cmp.w d5,d3
 blt.b $C2E9DC
 move.w (a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 cmp.w d2,d6
 ble.b $C2E9DC
 bsr.w $C2EA5A
 beq.b $C2E986
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E9DC
 move.w d3,d6
 neg.w d6
 cmp.w d5,d6
 blt.b $C2E9DC
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E9DC
 bsr.w $C2EAD0
 bne.b $C2E9DC
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.b $C2E9F8
