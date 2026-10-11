; Byte-exact callback match stage at $C10228-$C10271.

                org     $C10228

POST_GATE_INPUT_A               equ $C458A0
POST_GATE_INPUT_B               equ $C458A1
POST_GATE_DELAY                 equ $C45AD6
POST_GATE_ACTIVE_FLAG           equ $C45795
POST_GATE_STATUS_FLAG           equ $C45858
POST_GATE_CONTEXT_BYTE          equ $C458AE
POST_GATE_MODE_LATCH            equ $C458AD
POST_GATE_CALLBACK_SLOT         equ $C1820C

advance_post_gate_match_callback:
                move.b  POST_GATE_INPUT_A.l,d0
                move.b  POST_GATE_INPUT_B.l,d1
                cmp.b   d1,d0
                bne.b   finish_post_gate_match_callback
                move.w  #2,POST_GATE_DELAY.l
                moveq   #1,d0
                move.b  d0,POST_GATE_ACTIVE_FLAG.l
                move.b  #$FF,POST_GATE_STATUS_FLAG.l
                move.b  #4,POST_GATE_CONTEXT_BYTE.l
                move.b  d0,POST_GATE_MODE_LATCH.l
                move.w  #5,POST_GATE_DELAY.l
                lea.l   post_gate_context_callback(pc),a0
                move.l  a0,POST_GATE_CALLBACK_SLOT.l

finish_post_gate_match_callback:
                rts

post_gate_context_callback       equ $C10678
