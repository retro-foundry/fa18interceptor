; Byte-exact keyboard-event source $C16BF2-$C16C39.
; Called by the raw keyboard poll at $C16C56.

                org     $C16BF2

EVENT_SOURCE_HANDLE             equ $C0815C
EVENT_SOURCE_DESCRIPTOR         equ $C1ABAC
EVENT_SOURCE_RELEASE_HANDLE     equ $C08134
CALL_C53C08                     equ $C53C08
CALL_C53C8C                     equ $C53C8C
DESCRIPTOR_EVENT_WORD           equ 6

read_keyboard_event_source:
                link.w  a6,#-6
                move.l  EVENT_SOURCE_HANDLE.l,-(a7)
                jsr     CALL_C53C08.l
                addq.l  #4,a7
                move.l  d0,-6(a6)
                tst.l   d0
                bne.b   consume_keyboard_event
                moveq   #0,d0
                unlk    a6
                rts

consume_keyboard_event:
                movea.l EVENT_SOURCE_DESCRIPTOR.l,a0
                move.w  DESCRIPTOR_EVENT_WORD(a0),d0
                clr.w   DESCRIPTOR_EVENT_WORD(a0)
                move.l  EVENT_SOURCE_RELEASE_HANDLE.l,-(a7)
                move.b  d0,-1(a6)
                jsr     CALL_C53C8C.l
                addq.l  #4,a7
                move.b  -1(a6),d0
                unlk    a6
                rts
