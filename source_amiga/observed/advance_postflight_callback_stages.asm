; Byte-exact postflight callback entries $C1104C-$C110A3.
; Their runtime use in run060 is not yet directly bounded.

                org     $C1104C

POSTFLIGHT_DELAY                equ     $C45AD6
POSTFLIGHT_AUXILIARY_FLAG       equ     $C45795
POSTFLIGHT_SEQUENCE_HEAD        equ     $C4574A
POSTFLIGHT_SEQUENCE_COUNTER     equ     $C457E0
POSTFLIGHT_EVENT_FLAG           equ     $C457AE
POSTFLIGHT_MODE_LATCH           equ     $C458AD
POSTFLIGHT_CALLBACK_SLOT        equ     $C1820C

postflight_callback_stage_one:
                move.w  POSTFLIGHT_DELAY.l,d0
                tst.w   d0
                bpl.b   .return
                moveq   #0,d0
                move.b  d0,POSTFLIGHT_AUXILIARY_FLAG.l
                move.w  #$000E,POSTFLIGHT_SEQUENCE_HEAD.l
                move.b  d0,POSTFLIGHT_SEQUENCE_COUNTER.l
                move.l  #$C118FC,POSTFLIGHT_CALLBACK_SLOT.l
.return:
                rts

postflight_callback_stage_two:
                move.w  POSTFLIGHT_DELAY.l,d0
                tst.w   d0
                bpl.b   .return
                moveq   #1,d0
                move.b  d0,POSTFLIGHT_EVENT_FLAG.l
                move.w  #2,POSTFLIGHT_DELAY.l
                lea.l   $C110A4(pc),a0
                move.l  a0,POSTFLIGHT_CALLBACK_SLOT.l
                move.b  d0,POSTFLIGHT_MODE_LATCH.l
.return:
                rts
