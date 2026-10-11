; Byte-exact observed message-sequence state guards $C32E1A-$C32E2D.
; Divergent branch targets remain raw.

                org     $C32E1A

MESSAGE_SEQUENCE_STATE_WORD     equ     $C45744
MESSAGE_SEQUENCE_MODE           equ     $C457E0
RETURN_EMPTY_SELECTOR           equ     $C32CC4
CONTINUE_MESSAGE_EVENT_GUARD    equ     $C32E5C

guard_message_sequence_state:
                tst.w   MESSAGE_SEQUENCE_STATE_WORD.l
                bgt.w   RETURN_EMPTY_SELECTOR
                cmpi.b  #2,MESSAGE_SEQUENCE_MODE.l
                bge.b   CONTINUE_MESSAGE_EVENT_GUARD
