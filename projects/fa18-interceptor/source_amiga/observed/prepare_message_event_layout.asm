; Byte-exact message-event layout preparation $C32EF6-$C32F5B.
; Queue/control ownership remains unresolved.

                org     $C32EF6

MESSAGE_RECORD_SUBTYPE           equ     $C457DB
MESSAGE_LAYOUT_STATE_C           equ     $C45748
MESSAGE_AUXILIARY_BYTE           equ     $C457DE
MESSAGE_EVENT_CODE               equ     $C457F5
MESSAGE_EVENT_CURSOR             equ     $C457F6
MESSAGE_EVENT_BUFFER_B           equ     $C457EB
MESSAGE_LAYOUT_TRIPLET           equ     $C456FE
MESSAGE_EVENT_EXTERNAL_POINTER   equ     $C1AB74
MESSAGE_EVENT_CODE_SPECIAL       equ     $41
MESSAGE_STATIC_TEXT_SETUP        equ     $C32F54
MESSAGE_STATIC_TEXT_COMPOSITOR   equ     $C32F5C

prepare_message_event_layout:
                move.b  MESSAGE_RECORD_SUBTYPE.l,d1
                ext.w   d1
                move.w  d1,MESSAGE_LAYOUT_STATE_C.l
                move.b  #1,MESSAGE_AUXILIARY_BYTE.l
                cmpi.b  #MESSAGE_EVENT_CODE_SPECIAL,d4
                bne.b   MESSAGE_STATIC_TEXT_SETUP
                tst.b   MESSAGE_EVENT_CURSOR.l
                ble.b   MESSAGE_STATIC_TEXT_SETUP
                subq.b  #1,MESSAGE_EVENT_CURSOR.l
                addq.b  #1,MESSAGE_EVENT_CODE.l
                movea.l MESSAGE_EVENT_EXTERNAL_POINTER.l,a0
                ; ADDA.W #$1E,A0; retain original immediate encoding.
                dc.w    $D0FC,$001E
                move.b  MESSAGE_EVENT_CURSOR.l,d5
                ext.w   d5
                clr.b   (a0,d5.w)
                lea.l   MESSAGE_EVENT_BUFFER_B.l,a0
                adda.w  d5,a0
                clr.b   (a0)+
                clr.b   (a0)
                movem.l MESSAGE_LAYOUT_TRIPLET.l,a1-a2/a4
                subq.w  #4,a1
                bra.b   MESSAGE_STATIC_TEXT_COMPOSITOR
