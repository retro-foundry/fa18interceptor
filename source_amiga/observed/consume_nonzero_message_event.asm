; Byte-exact nonzero message-event consumer $C32E7C-$C32EAD.
; Event-code semantics and display ownership remain unresolved.

                org     $C32E7C

MESSAGE_AUXILIARY_BYTE           equ     $C457DE
MESSAGE_EVENT_BYTES              equ     $C457E1
MESSAGE_EVENT_BYTE_INDEX         equ     $C457F8
MESSAGE_EVENT_COUNT              equ     $C457F9
MESSAGE_EVENT_CODE_SPECIAL       equ     $44
MESSAGE_EVENT_OTHER_PATH         equ     $C32EF6
MESSAGE_EVENT_SPECIAL_PATH       equ     $C32EAE
MESSAGE_STATIC_TEXT_SETUP        equ     $C32F54

consume_nonzero_message_event:
                cmpi.b  #MESSAGE_EVENT_CODE_SPECIAL,d4
                beq.b   MESSAGE_EVENT_SPECIAL_PATH
                tst.b   MESSAGE_AUXILIARY_BYTE.l
                ble.b   MESSAGE_EVENT_OTHER_PATH
                subq.b  #1,MESSAGE_AUXILIARY_BYTE.l
                subq.b  #1,MESSAGE_EVENT_COUNT.l
                clr.b   (a0,d5.w)
                addq.b  #1,d5
                cmpi.b  #10,d5
                blt.b   .publish_event_index
                clr.b   d5
.publish_event_index:
                move.b  d5,MESSAGE_EVENT_BYTE_INDEX.l
                ; BRA.W $C32F54; retain original word-branch encoding.
                dc.w    $6000,$00A8

