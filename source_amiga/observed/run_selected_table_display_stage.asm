; Byte-exact $C2FEDE-$C2FF45 display-stage routine.
; The observed path is return-bounded at $C0D742. Structure ownership and the
; semantic meaning of the copied words are not established.
                org $C2FEDE
SAVED_TABLE_FIELD equ $C456E2
SELECTOR equ $C4589B
MODE_FLAG equ $C45785
SOURCE_RECORDS equ $C4B390
OUTPUT_RECORDS equ $C4B432
prepare_selected_table_display_stage:
 move.l SAVED_TABLE_FIELD.l,-(sp)
 move.l 12(a2),SAVED_TABLE_FIELD.l
 jsr $C0D752.l
 bne.b .restore_table
 bsr.w $C301F6
 bne.b .restore_table
 tst.b SELECTOR.l
 beq.b .restore_table
 moveq #8,d0
 moveq #0,d3
 moveq #0,d4
 bsr.w $C30466
.restore_table:
 move.l (sp)+,SAVED_TABLE_FIELD.l
 tst.b MODE_FLAG.l
 bne.b .clear_output
 jsr $C0D74A.l
 bne.b .clear_output
 lea SOURCE_RECORDS.l,a0
 lea OUTPUT_RECORDS.l,a4
 move.w (a0)+,(a4)+
 move.l (a0)+,(a4)+
 move.l (a0)+,(a4)+
 move.l (a0)+,(a4)+
 move.l (a0)+,(a4)+
 move.l (a0)+,(a4)+
 rts
.clear_output:
 move.w #0,OUTPUT_RECORDS.l
 rts
