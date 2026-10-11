; Byte-exact structural event/update stage $C16EAE-$C16F1B.
; The external objects, code values, and field meanings remain unassigned.
                org     $C16EAE

EXTERNAL_EVENT_ARGUMENT         equ $C1ABEC
EXTERNAL_EVENT_RECORD           equ $C1ABCA
EXTERNAL_EVENT_FOLLOWUP         equ $C1ABCE
EXTERNAL_EVENT_EMPTY_FLAG       equ $C4582D
EVENT_RECORD_CODE_OFFSET         equ $06
EVENT_CODE_FIRST                equ $68
EVENT_CODE_SECOND               equ $E8
READ_EXTERNAL_EVENT             equ $C53C08
DISPATCH_FIRST_EVENT_CODE       equ $C0833E
DISPATCH_SECOND_EVENT_CODE      equ $C08394
FINISH_EXTERNAL_EVENT           equ $C53C8C
UPDATE_HARDWARE_STATE           equ $C16F1C

refresh_external_event_and_dispatch_codes:
                link    a6,#-6
                move.l  EXTERNAL_EVENT_ARGUMENT.l,-(sp)
                jsr     READ_EXTERNAL_EVENT.l
                addq.l  #4,sp
                move.l  d0,-6(a6)
                tst.l   d0
                bne.s   .dispatch_pending_code
                clr.b   EXTERNAL_EVENT_EMPTY_FLAG.l
                bra.s   .update_hardware_state
.dispatch_pending_code:
                movea.l EXTERNAL_EVENT_RECORD.l,a0
                move.w  EVENT_RECORD_CODE_OFFSET(a0),d0
                move.w  d0,-2(a6)
                tst.w   d0
                beq.s   .clear_consumed_code
                cmpi.w  #EVENT_CODE_FIRST,d0
                bne.s   .check_second_code
                jsr     DISPATCH_FIRST_EVENT_CODE.l
                bra.s   .clear_consumed_code
.check_second_code:
                cmpi.w  #EVENT_CODE_SECOND,-2(a6)
                bne.s   .clear_consumed_code
                jsr     DISPATCH_SECOND_EVENT_CODE.l
.clear_consumed_code:
                movea.l EXTERNAL_EVENT_RECORD.l,a0
                clr.w   EVENT_RECORD_CODE_OFFSET(a0)
                move.l  EXTERNAL_EVENT_FOLLOWUP.l,-(sp)
                jsr     FINISH_EXTERNAL_EVENT.l
                addq.l  #4,sp
.update_hardware_state:
                bsr.s   UPDATE_HARDWARE_STATE
                unlk    a6
                rts
