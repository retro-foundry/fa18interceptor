; Byte-exact timer-gated post-input callback $C0FA04-$C0FA4B.

                org     $C0FA04

CALL_C0FAA4                     equ $C0FAA4
CALL_C2FD22                     equ $C2FD22
POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_COMMAND_MODE           equ $C4584B
POST_TICK_AUXILIARY_BYTE         equ $C45785
POST_TICK_COUNTDOWN              equ $C45AD6
POST_TICK_MODE_BYTE              equ $C458A1
POST_TICK_INPUT_A                equ $C458A0

callback_finish_followup:
                move.w  POST_TICK_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   update_callback_while_waiting
                bsr.w   CALL_C0FAA4
                move.b  #3,POST_TICK_COMMAND_MODE.l
                moveq   #0,d0
                move.b  d0,POST_TICK_AUXILIARY_BYTE.l
                move.w  #2,POST_TICK_COUNTDOWN.l
                move.b  #$0F,POST_TICK_MODE_BYTE.l
                move.b  d0,POST_TICK_INPUT_A.l
                lea.l   callback_after_finish_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l
                bra.b   finish_callback_finish_followup

update_callback_while_waiting:
                jsr     CALL_C2FD22.l

finish_callback_finish_followup:
                rts

callback_after_finish_followup    equ $C0FA4C
