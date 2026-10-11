; Byte-exact map-transition callback $C0F992-$C0FA03.

                org     $C0F992

CALL_CONTEXT_HELPERS             equ $C0F4A6
CALL_C08F26                     equ $C08F26
CALL_C11ACC                     equ $C11ACC
POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_ENABLE                 equ $C458A6
POST_TICK_COMMAND_MODE           equ $C4584B
POST_TICK_EVENT_FLAG             equ $C457AE
POST_TICK_COUNTDOWN              equ $C45AD6
POST_TICK_MODE_BYTE              equ $C458A1
C11ACC_ARGUMENT                  equ $C08490

callback_begin_followup:
                jsr     CALL_CONTEXT_HELPERS.l
                jsr     CALL_C08F26.l
                move.b  POST_TICK_ENABLE.l,d0
                subq.b  #2,d0
                bne.b   check_followup_command_mode
                move.b  #$7D,POST_TICK_ENABLE.l

check_followup_command_mode:
                move.b  POST_TICK_COMMAND_MODE.l,d0
                subq.b  #3,d0
                bne.b   initialize_followup_callback
                move.b  #1,POST_TICK_EVENT_FLAG.l
                move.w  #2,POST_TICK_COUNTDOWN.l
                clr.b   POST_TICK_MODE_BYTE.l
                move.b  #$7F,POST_TICK_ENABLE.l
                lea.l   callback_finish_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l
                bra.b   finish_followup_callback

initialize_followup_callback:
                clr.b   POST_TICK_COMMAND_MODE.l
                pea.l   C11ACC_ARGUMENT.l
                jsr     CALL_C11ACC.l
                addq.l  #4,a7
                lea.l   callback_after_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_followup_callback:
                rts

callback_finish_followup          equ $C0FA04
callback_after_followup           equ $C0FCB4
