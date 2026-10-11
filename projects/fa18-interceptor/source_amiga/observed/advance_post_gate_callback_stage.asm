; Byte-exact delayed callback stage at $C101FC-$C10227.

                org     $C101FC

POST_GATE_DELAY                 equ $C45AD6
POST_GATE_ACTIVE_FLAG           equ $C45795
POST_GATE_INPUT_A               equ $C458A0
POST_GATE_INPUT_B               equ $C458A1
POST_GATE_CALLBACK_SLOT         equ $C1820C

advance_post_gate_callback_stage:
                move.w  POST_GATE_DELAY.l,d0
                tst.w   d0
                bpl.b   finish_post_gate_callback_stage
                moveq   #0,d0
                move.b  d0,POST_GATE_ACTIVE_FLAG.l
                move.b  #$F,POST_GATE_INPUT_B.l
                move.b  d0,POST_GATE_INPUT_A.l
                lea.l   post_gate_wait_for_input_match(pc),a0
                move.l  a0,POST_GATE_CALLBACK_SLOT.l

finish_post_gate_callback_stage:
                rts

post_gate_wait_for_input_match  equ $C10228
