; Byte-exact $C0D9EA-$C0DA37 sixth record-write branch of the pair selector.
                org $C0D9EA
DISPLAY_RECORDS equ $C4B390
SEQUENCE_FLAGS equ $C458CA
emit_sixth_display_record_pair:
 lea DISPLAY_RECORDS.l,a1
 move.w #5,(a1)+
 move.w SEQUENCE_FLAGS.l,d6
 andi.w #2,d6
 bne.b .extended_sequence
.short_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DAD4
 bsr.w $C0DAD0
 bsr.w $C0DAE6
 bra.w $C0DA70
.extended_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DAD4
 bsr.w $C0DAD0
 bsr.w $C0DAE6
 lea DISPLAY_RECORDS.l,a1
 move.w #3,(a1)+
 addq.w #8,a1
 bsr.w $C0DADC
 bra.w $C0DA70
