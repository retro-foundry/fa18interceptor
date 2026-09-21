; Byte-exact $C0DA38-$C0DA6F fallback record-write and threshold branch.
                org $C0DA38
DISPLAY_RECORDS equ $C4B390
MODE_FLAG equ $C45785
MODE_ZERO_THRESHOLD_SOURCE equ $C45A8A
MODE_NONZERO_THRESHOLD_SOURCE equ $C45A94
emit_fallback_display_record_pair:
 lea DISPLAY_RECORDS.l,a1
 move.w #4,(a1)+
 bsr.w $C0DAD0
 bsr.w $C0DAD4
 bsr.w $C0DADC
 bsr.w $C0DAE6
 tst.b MODE_FLAG.l
 bne.b .mode_nonzero
 move.w MODE_ZERO_THRESHOLD_SOURCE.l,d0
 bra.b .test_threshold
.mode_nonzero:
 move.w MODE_NONZERO_THRESHOLD_SOURCE.l,d0
.test_threshold:
 cmpi.w #$3840,d0
 bgt.b $C0DA70
 bra.b $C0DA94
