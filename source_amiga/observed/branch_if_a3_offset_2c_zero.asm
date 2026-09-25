; Byte-exact delayed record-update sequence at $C50212-$C5027B.
; The observed P-code path takes the zero-countdown return. The nonzero body
; is reconstructed from the same runtime image; record ownership and command
; meanings remain unassigned.
                org     $C50212

RECORD_UPDATE_COUNTDOWN         equ $2C
RECORD_UPDATE_COMMAND_BASE      equ $30
RECORD_UPDATE_COMMAND_CURSOR    equ $34
RECORD_UPDATE_SUBCOUNT_BASE     equ $24
RECORD_UPDATE_TERMINATOR        equ $2C
RECORD_UPDATE_SUBCOMMAND_BASE   equ $40
RECORD_UPDATE_OWNER_CLEAR       equ $0
RECORD_UPDATE_COMPLETE_CALLBACK equ $C4FFB4

apply_delayed_record_updates:
                ; Preserve CMPi.L #0,(d16,A3); vasm otherwise emits TST.L.
                dc.w    $0CAB,0,0,RECORD_UPDATE_COUNTDOWN
                beq.s   .return
                subq.l  #1,RECORD_UPDATE_COUNTDOWN(a3)
                bne.s   .return
                movea.l RECORD_UPDATE_COMMAND_CURSOR(a3),a0
.relative_command_cursor:
                adda.l  RECORD_UPDATE_COMMAND_BASE(a3),a0
.next_command:
                movea.l (a0)+,a4
                move.l  (a0)+,d0
                ; Preserve the original long-immediate CMPA encoding.
                dc.w    $B9FC,0,RECORD_UPDATE_TERMINATOR
                bge.s   .command_control
                adda.l  a3,a4
                ; Original uses the explicit zero-displacement A4 form.
                dc.w    $2940,0
                bra.s   .next_command
.command_control:
                beq.s   .install_countdown
                move.w  a4,d1
                subi.w  #RECORD_UPDATE_SUBCOMMAND_BASE,d1
                ; Preserve CMPI.L #0,(d8,A3,D1.W); vasm otherwise emits TST.L.
                dc.w    $0CB3,0,0,$1024
                beq.s   .follow_command_pointer
                subq.l  #1,RECORD_UPDATE_SUBCOUNT_BASE(a3,d1.w)
                beq.s   .next_command
.follow_command_pointer:
                movea.l d0,a0
                bra.s   .relative_command_cursor
.install_countdown:
                move.l  d0,RECORD_UPDATE_COUNTDOWN(a3)
                suba.l  RECORD_UPDATE_COMMAND_BASE(a3),a0
                move.l  a0,RECORD_UPDATE_COMMAND_CURSOR(a3)
                ; Preserve CMPI.L #0,D0 rather than VASM's TST.L optimization.
                dc.w    $0C80,0,0
                bne.s   .return
                ; Original uses the explicit zero-displacement A2 form.
                dc.w    $2540,RECORD_UPDATE_OWNER_CLEAR
                move.l  d3,d0
                jsr     RECORD_UPDATE_COMPLETE_CALLBACK.l
.return:
                rts
