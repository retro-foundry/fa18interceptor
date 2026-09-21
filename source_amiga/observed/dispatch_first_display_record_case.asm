; Byte-exact $C2E7D4-$C2E7FB first case block of the C2E758 record iterator.
                org $C2E7D4
CASE_SIGNED_WORD equ $C45ACA
CASE_COUNTER equ $C4E85A
CASE_RESULT_SLOT equ $C4E862
dispatch_first_display_record_case:
 beq.w $C2E986
 tst.w CASE_SIGNED_WORD.l
 bge.b $C2E834
 neg.w d2
 cmp.w d2,d6
 ble.b $C2E834
 bsr.w $C2EAD0
 bne.b $C2E834
 addq.w #1,CASE_COUNTER.l
 move.w d0,CASE_RESULT_SLOT.l
 bra.w $C2E9F8
