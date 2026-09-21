; Byte-exact $C0D95A-$C0D9A7 fourth record-write branch of the pair selector.
                org $C0D95A
DISPLAY_RECORDS equ $C4B390
SEQUENCE_FLAGS equ $C458CA
emit_fourth_display_record_pair:
 lea DISPLAY_RECORDS.l,a1
 move.w #5,(a1)+
 move.w SEQUENCE_FLAGS.l,d6
 andi.w #2,d6
 bne.b .short_sequence
.extended_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DADC
 bsr.w $C0DAE6
 bsr.w $C0DAD0
 lea DISPLAY_RECORDS.l,a1
 move.w #3,(a1)+
 addq.w #8,a1
 bsr.w $C0DAD4
 bra.w $C0DA70
.short_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DADC
 bsr.w $C0DAE6
 bsr.w $C0DAD0
 bra.w $C0DA70
