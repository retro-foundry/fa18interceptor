; Byte-exact $C0D9A8-$C0D9E9 threshold-controlled record-write branch.
                org $C0D9A8
DISPLAY_RECORDS equ $C4B390
THRESHOLD_SOURCE equ $C45A92
emit_threshold_display_record_pair:
 lea DISPLAY_RECORDS.l,a1
 move.w #4,(a1)+
 cmpi.w #$3840,THRESHOLD_SOURCE.l
 bge.b .short_sequence
.extended_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DAE6
 bsr.w $C0DAD0
 lea $C4B39A.l,a1
 bsr.w $C0DADC
 bsr.w $C0DAD4
 bra.w $C0DA70
.short_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DAE6
 bsr.w $C0DAD0
 bra.w $C0DA70
