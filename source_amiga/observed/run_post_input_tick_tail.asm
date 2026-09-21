; Byte-exact observed $C0F5F8 tail $C0F7D2-$C0F811.
; Bounded in the no-input training frame-600 packet.

                org     $C0F7D2

POST_INPUT_GUARD                equ $C4582C
POST_INPUT_TICK_COUNTER         equ $C457C1
POST_INPUT_COUNTDOWN            equ $C45AD6
POST_INPUT_CALLBACK_POINTER     equ $C1820C
COMMAND_INPUT_PENDING           equ $C457A3

run_post_input_tick_tail:
                move.b  POST_INPUT_GUARD.l,d0
                tst.b   d0
                bmi.s   .update_tick
                subq.b  #1,d0
                move.b  d0,POST_INPUT_GUARD.l
.update_tick:
                move.b  POST_INPUT_TICK_COUNTER.l,d0
                addq.b  #1,d0
                move.b  d0,POST_INPUT_TICK_COUNTER.l
                move.w  POST_INPUT_COUNTDOWN.l,d0
                subq.w  #1,d0
                move.w  d0,POST_INPUT_COUNTDOWN.l
                movea.l POST_INPUT_CALLBACK_POINTER.l,a0
                jsr     (a0)
                clr.b   COMMAND_INPUT_PENDING.l
                unlk    a6
                rts
