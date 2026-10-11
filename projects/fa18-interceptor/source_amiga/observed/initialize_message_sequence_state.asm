; Byte-exact state initializer called by the nonzero path at $C1075A.
; It clears only the first two selector words; subsequent sequence population
; is outside this bounded slice.

                org     $C11312

MESSAGE_SELECTOR_SEQUENCE        equ $C4574A
MESSAGE_SEQUENCE_CURSOR          equ $C457C6
MESSAGE_ACTIVE_FLAG              equ $C457C3
MESSAGE_EFFECT_COUNTER           equ $C457E0
MESSAGE_INHIBIT_FLAG             equ $C45871
MESSAGE_DELAY                    equ $C4573E

initialize_message_sequence_state:
                link.w  a6,#-4
                lea.l   MESSAGE_SELECTOR_SEQUENCE.l,a0
                moveq   #0,d0
                move.w  d0,(a0)
                addq.l  #2,a0
                move.w  d0,(a0)
                addq.l  #2,a0
                move.l  #$1B8,MESSAGE_DELAY.l
                moveq   #0,d0
                move.b  d0,MESSAGE_SEQUENCE_CURSOR.l
                move.b  d0,MESSAGE_ACTIVE_FLAG.l
                move.b  d0,MESSAGE_EFFECT_COUNTER.l
                move.b  d0,MESSAGE_INHIBIT_FLAG.l
                unlk    a6
                rts
