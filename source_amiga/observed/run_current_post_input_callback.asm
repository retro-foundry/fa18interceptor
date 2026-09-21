; Byte-exact callback reached by the training-frame post-input trace $C1075A-$C10789.

                org     $C1075A

POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_MODE_FLAG              equ $C458AC
POST_TICK_AUXILIARY_BYTE         equ $C45785
POST_TICK_MODE_LATCH             equ $C458AD
POST_TICK_COUNTDOWN              equ $C45AD6
CALL_C11312                     equ $C11312

run_current_post_input_callback:
                tst.b   POST_TICK_MODE_FLAG.l
                beq.b   finish_current_post_input_callback
                tst.b   POST_TICK_AUXILIARY_BYTE.l
                beq.b   call_current_post_input_helper
                move.b  #2,POST_TICK_MODE_LATCH.l

call_current_post_input_helper:
                bsr.w   CALL_C11312
                move.w  #3,POST_TICK_COUNTDOWN.l
                lea.l   callback_after_current_helper(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_current_post_input_callback:
                rts

callback_after_current_helper     equ $C1078A
