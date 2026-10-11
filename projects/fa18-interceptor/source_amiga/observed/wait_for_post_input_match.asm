; Byte-exact post-input input-match callback $C0FA4C-$C0FA7F.

                org     $C0FA4C

POST_TICK_CALLBACK_SLOT          equ $C1820C
POST_TICK_COUNTDOWN              equ $C45AD6
POST_TICK_AUXILIARY_BYTE         equ $C45795
POST_TICK_INPUT_A                equ $C458A0
POST_TICK_INPUT_B                equ $C458A1

callback_after_finish_followup:
                move.w  POST_TICK_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   finish_callback_after_followup
                clr.b   POST_TICK_AUXILIARY_BYTE.l
                move.b  POST_TICK_INPUT_A.l,d0
                move.b  POST_TICK_INPUT_B.l,d1
                cmp.b   d1,d0
                bne.b   finish_callback_after_followup
                move.w  #2,POST_TICK_COUNTDOWN.l
                lea.l   callback_complete_followup(pc),a0
                move.l  a0,POST_TICK_CALLBACK_SLOT.l

finish_callback_after_followup:
                rts

callback_complete_followup        equ $C0FA80
