; Byte-exact static-text compositor prelude $C32F54-$C32FCD.
; Message/event and display-field ownership remain unresolved.

                org     $C32F54

MESSAGE_LAYOUT_TRIPLET           equ     $C456FE
MESSAGE_SEQUENCE_MODE            equ     $C457E0
MESSAGE_AUXILIARY_BYTE           equ     $C457DE
MESSAGE_EVENT_CODE               equ     $C457F5
MESSAGE_EVENT_CURSOR             equ     $C457F6
MESSAGE_EVENT_EXTERNAL_POINTER   equ     $C1AB74
STATIC_TEXT_LAYOUT_TABLE         equ     $C3D8FC
STATIC_TEXT_RENDER_POINTERS      equ     $C456B6
STATIC_TEXT_EVENT_MAP            equ     $C331CE
STATIC_TEXT_GLYPH_SETUP          equ     $C32FCE
STATIC_TEXT_REJECT               equ     $C330F4
STATIC_TEXT_EVENT_TAIL           equ     $C32FD8

prepare_static_text_compositor:
                movem.l MESSAGE_LAYOUT_TRIPLET.l,a1-a2/a4
                lea.l   STATIC_TEXT_LAYOUT_TABLE.l,a3
                movea.l STATIC_TEXT_RENDER_POINTERS.l,a5
                move.w  #$1C2,d7
                cmpi.b  #2,MESSAGE_SEQUENCE_MODE.l
                blt.b   STATIC_TEXT_GLYPH_SETUP
                tst.b   d4
                beq.w   STATIC_TEXT_REJECT
                andi.w  #$FF,d4
                lea.l   STATIC_TEXT_EVENT_MAP.l,a0
                move.b  (a0,d4.w),d4
                beq.w   STATIC_TEXT_REJECT
                cmpi.b  #$20,d4
                blt.b   STATIC_TEXT_EVENT_TAIL
                tst.b   MESSAGE_AUXILIARY_BYTE.l
                ble.b   STATIC_TEXT_EVENT_TAIL
                cmpi.b  #3,MESSAGE_SEQUENCE_MODE.l
                beq.b   .advance_event_cursor
                movea.l MESSAGE_EVENT_EXTERNAL_POINTER.l,a0
                ; ADDA.W #$1E,A0; retain original immediate encoding.
                dc.w    $D0FC,$001E
                move.b  MESSAGE_EVENT_CURSOR.l,d5
                ext.w   d5
                move.b  d4,(a0,d5.w)
.advance_event_cursor:
                addq.b  #1,MESSAGE_EVENT_CURSOR.l
                subq.b  #1,MESSAGE_EVENT_CODE.l
                bge.b   STATIC_TEXT_EVENT_TAIL
                moveq   #$20,d4
                bra.b   STATIC_TEXT_EVENT_TAIL

