; Byte-exact callback chain installed by the post-input tick $C0F920-$C0F991.

                org     $C0F920

CALL_C08F26                     equ $C08F26
POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_MODE_FLAG              equ $C458AC
POST_TICK_ENABLE                 equ $C458A6
POST_TICK_COMMAND_MODE           equ $C4584B
POST_TICK_COUNTDOWN              equ $C45AD6
POST_TICK_INPUT_A                equ $C458A0
POST_TICK_INPUT_B                equ $C458A1
POST_TICK_AUXILIARY_BYTE         equ $C45795

callback_clear_post_tick_state:
                jsr     CALL_C08F26.l
                moveq   #0,d0
                move.b  d0,POST_TICK_MODE_FLAG.l
                move.b  d0,POST_TICK_ENABLE.l
                move.b  d0,POST_TICK_COMMAND_MODE.l
                lea.l   callback_prepare_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l
                rts

callback_wait_for_input_match:
                move.w  POST_TICK_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   finish_callback_wait
                move.b  POST_TICK_INPUT_A.l,d0
                move.b  POST_TICK_INPUT_B.l,d1
                cmp.b   d1,d0
                bne.b   finish_callback_wait
                move.w  #2,POST_TICK_COUNTDOWN.l
                lea.l   callback_set_auxiliary_byte(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_callback_wait:
                rts

callback_set_auxiliary_byte:
                move.w  POST_TICK_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   finish_callback_auxiliary
                move.b  #1,POST_TICK_AUXILIARY_BYTE.l
                lea.l   callback_begin_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_callback_auxiliary:
                rts

callback_prepare_followup          equ $C0FBE0
callback_begin_followup            equ $C0F992
