; Byte-exact $C0D872-$C0D8BD first record-write branch of the pair selector.
                org $C0D872
DISPLAY_RECORDS equ $C4B390
MODE_FLAG equ $C45785
SEQUENCE_FLAGS equ $C458CA
emit_first_display_record_pair:
 lea DISPLAY_RECORDS.l,a1
 move.w #4,(a1)+
 tst.b MODE_FLAG.l
 bne.b .extended_sequence
 move.w SEQUENCE_FLAGS.l,d6
 andi.w #2,d6
 bne.b .short_sequence
.extended_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DADC
 bsr.w $C0DAE6
 lea $C4B39A.l,a1
 bsr.w $C0DAD4
 bsr.w $C0DAD0
 bra.w $C0DA70
.short_sequence:
 bsr.w $C0DAA0
 bsr.w $C0DADC
 bsr.w $C0DAE6
 bra.w $C0DA70
