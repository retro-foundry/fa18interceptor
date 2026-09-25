; Byte-exact special message-event mode dispatch $C32EAE-$C32EE3.
; The external helper purposes and state meaning are unresolved.

                org     $C32EAE

MESSAGE_SEQUENCE_MODE            equ     $C457E0
MESSAGE_EVENT_LATCH              equ     $C457D5
MESSAGE_EVENT_SPECIAL_HELPER     equ     $C25246
MESSAGE_EVENT_FALLBACK_HELPER    equ     $C1643A
RETURN_MESSAGE_EVENT             equ     $C32CEC

dispatch_message_event_special_mode:
                cmpi.b  #3,MESSAGE_SEQUENCE_MODE.l
                bne.b   .fallback
                jsr     MESSAGE_EVENT_SPECIAL_HELPER.l
                ori.b   #$80,MESSAGE_SEQUENCE_MODE.l
                ; BRA.W $C32CEC; retain original word-branch encoding.
                dc.w    $6000,$FE24
.fallback:
                andi.b  #$FD,MESSAGE_SEQUENCE_MODE.l
                jsr     MESSAGE_EVENT_FALLBACK_HELPER.l
                move.b  #$FF,MESSAGE_EVENT_LATCH.l
                ; BRA.W $C32CEC; retain original word-branch encoding.
                dc.w    $6000,$FE0A

