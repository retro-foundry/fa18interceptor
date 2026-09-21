; Byte-exact $C2E94C-$C2E993 first D3-signed case block of C2E758.
                org $C2E94C
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E856
CASE_RESULT_SLOT equ $C4E85E
dispatch_seventh_display_record_case:
 tst.w d3
 bge.b $C2E994
 move.w d3,d6
 neg.w d6
 cmp.w d5,d6
 blt.w $C2E9DC
 move.w (a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E9DC
 bsr.w $C2EAD0
 beq.b $C2E9CA
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E9DC
 cmp.w d5,d3
 blt.b $C2E9DC
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E9DC
 bsr.w $C2EA5A
 bne.b $C2E9DC
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.b $C2E9F8
