; Byte-exact auxiliary message-byte consumer $C32EE4-$C32EF5.

                org     $C32EE4

MESSAGE_AUXILIARY_BYTE           equ     $C457DE
RETURN_MESSAGE_EVENT             equ     $C32CEC
MESSAGE_STATIC_TEXT_SETUP        equ     $C32F54

consume_message_auxiliary_byte:
                tst.b   MESSAGE_AUXILIARY_BYTE.l
                ble.w   RETURN_MESSAGE_EVENT
                subq.b  #1,MESSAGE_AUXILIARY_BYTE.l
                bra.b   MESSAGE_STATIC_TEXT_SETUP

