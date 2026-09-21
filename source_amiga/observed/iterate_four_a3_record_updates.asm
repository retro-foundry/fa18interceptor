; Byte-exact structural record iterator $C50158-$C501DF.
; Record ownership and field meanings are unassigned.
                org     $C50158

RECORD_ITERATOR_BASE            equ $C4FE28
RECORD_ITERATION_COUNT          equ 4
A1_NEXT_RECORD_OFFSET           equ $04
A3_POSITION_X_OFFSET            equ $08
A3_POSITION_Y_OFFSET            equ $0C
A3_DELTA_X_OFFSET               equ $18
A3_DELTA_Y_OFFSET               equ $1C
A3_DELTA_X_COUNTDOWN_OFFSET     equ $38
A3_DELTA_Y_COUNTDOWN_OFFSET     equ $3C
GUARD_A3_RECORD                 equ $C50212
CLAMP_A3_HIGH_WORDS             equ $C501E0

iterate_four_a3_record_updates:
                movem.l d0-d3/a0-a4,-(sp)
                moveq   #0,d3
                lea.l   RECORD_ITERATOR_BASE.l,a0
.next_record:
                move.l  a0,-(sp)
                move.w  d3,-(sp)
                ; Preserve explicit zero-displacement pointer-load encodings.
                dc.w    $2268,0                 ; movea.l 0(a0),a1
                dc.w    $2469,A1_NEXT_RECORD_OFFSET ; movea.l 4(a1),a2
                dc.w    $266A,0                 ; movea.l 0(a2),a3
                dc.w    $B7FC,0,0               ; cmpa.l #0,a3
                beq.s   .advance_record
                jsr     GUARD_A3_RECORD.l
                dc.w    $2069,0                 ; movea.l 0(a1),a0
                jsr     CLAMP_A3_HIGH_WORDS.l
                move.l  A3_DELTA_X_OFFSET(a3),d0
                add.l   d0,A3_POSITION_X_OFFSET(a3)
                move.l  A3_DELTA_Y_OFFSET(a3),d0
                add.l   d0,A3_POSITION_Y_OFFSET(a3)
                ; Preserve CMPI.L #0 rather than VASM's TST.L optimization.
                dc.w    $0CAB,0,0,A3_DELTA_X_COUNTDOWN_OFFSET
                beq.s   .check_y_countdown
                subq.l  #1,A3_DELTA_X_COUNTDOWN_OFFSET(a3)
                bne.s   .check_y_countdown
                move.l  #0,A3_DELTA_X_OFFSET(a3)
.check_y_countdown:
                ; Preserve CMPI.L #0 rather than VASM's TST.L optimization.
                dc.w    $0CAB,0,0,A3_DELTA_Y_COUNTDOWN_OFFSET
                beq.s   .advance_record
                subq.l  #1,A3_DELTA_Y_COUNTDOWN_OFFSET(a3)
                bne.s   .advance_record
                move.l  #0,A3_DELTA_Y_OFFSET(a3)
.advance_record:
                move.w  (sp)+,d3
                movea.l (sp)+,a0
                addq.l  #4,a0
                addq.w  #1,d3
                cmpi.w  #RECORD_ITERATION_COUNT,d3
                bne.s   .next_record
                movem.l (sp)+,d0-d3/a0-a4
                rts
