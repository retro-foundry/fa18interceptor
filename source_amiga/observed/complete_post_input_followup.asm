; Byte-exact post-input completion callback $C0FA80-$C0FAA3.

                org     $C0FA80

POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_COUNTDOWN              equ $C45AD6
POST_TICK_EVENT_FLAG             equ $C457AE
POST_TICK_AUXILIARY_BYTE         equ $C45795

callback_complete_followup:
                move.w  POST_TICK_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   finish_callback_completion
                clr.b   POST_TICK_EVENT_FLAG.l
                move.b  #1,POST_TICK_AUXILIARY_BYTE.l
                lea.l   callback_continue_after_completion(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_callback_completion:
                rts

callback_continue_after_completion equ $C10C08
