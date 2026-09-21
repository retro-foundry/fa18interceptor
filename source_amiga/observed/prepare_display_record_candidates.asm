; Byte-exact bounded prefix $C0D752-$C0D7DF.
; It fills five spaced three-word outputs and calls the observed $C2E758 child.
                org $C0D752
MATRIX_WORDS equ $C45BD8
INPUT_TRIPLES equ $C0D720
OUTPUT_RECORDS equ $C4B390
SCRATCH_WORDS equ $C4E854
SOURCE_LONG equ $C45A66
prepare_display_record_candidates:
 lea MATRIX_WORDS.l,a2
 lea (a2),a4
 link a6,#-2
 lea INPUT_TRIPLES(pc),a1
 lea OUTPUT_RECORDS.l,a3
 lea -2(a6),a0
 move.w #4,(a0)
.next_input:
 move.w (a1)+,d2
 move.l SOURCE_LONG.l,d3
 swap d3
 asr.w #2,d3
 move.w (a1)+,d4
 lea (a4),a2
 move.w d2,d5
 move.w d3,d6
 move.w d4,d7
 muls.w (a2)+,d5
 muls.w (a2)+,d6
 muls.w (a2)+,d7
 add.l d6,d7
 add.l d5,d7
 asr.l #8,d7
 bcc.b .first_unrounded
 addq.w #1,d7
.first_unrounded:
 move.w d7,(a3)+
 move.w d2,d5
 move.w d3,d6
 move.w d4,d7
 muls.w (a2)+,d5
 muls.w (a2)+,d6
 muls.w (a2)+,d7
 add.l d6,d7
 add.l d5,d7
 asr.l #8,d7
 bcc.b .second_unrounded
 addq.w #1,d7
.second_unrounded:
 move.w d7,(a3)+
 muls.w (a2)+,d2
 muls.w (a2)+,d3
 muls.w (a2)+,d4
 add.l d3,d4
 add.l d2,d4
 asr.l #8,d4
 bcc.b .third_unrounded
 addq.w #1,d4
.third_unrounded:
 move.w d4,(a3)+
 move.w d4,d3
 dc.w $d6fc,$001a ; adda.w #$1a,a3; keep original non-relaxed opcode
 subq.w #1,(a0)
 bgt.b .next_input
 lea SCRATCH_WORDS.l,a4
 clr.l (a4)+
 clr.l (a4)+
 clr.l (a4)+
 clr.l (a4)
 jsr $C2E758.l
