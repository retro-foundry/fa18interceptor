; Byte-exact reconstruction of $C17456-$C1748B (Hunk 0 +$95A6).
; Registers the JOY0DAT delta callback descriptor with the runtime service.

                org     $C17456

JOYSTICK_CALLBACK_DESCRIPTOR       equ $C1ABF0
JOYSTICK_CALLBACK_KIND             equ $C1ABF8
JOYSTICK_CALLBACK_SUBTYPE          equ $C1ABF9
JOYSTICK_CALLBACK_BOUNDS           equ $C1ABFA
JOYSTICK_CALLBACK_ENTRY            equ $C1AC02
JOYSTICK_CALLBACK_BOUNDS_VALUE     equ $C081B4
REGISTER_CALLBACK_DESCRIPTOR       equ $C53B00
CALLBACK_DESCRIPTOR_KIND           equ 5
JOYSTICK_CALLBACK_KIND_VALUE       equ 2

register_joystick_delta_callback:
                move.b  #JOYSTICK_CALLBACK_KIND_VALUE,JOYSTICK_CALLBACK_KIND.l
                clr.b   JOYSTICK_CALLBACK_SUBTYPE.l
                move.l  #JOYSTICK_CALLBACK_BOUNDS_VALUE,JOYSTICK_CALLBACK_BOUNDS.l
                lea     joystick_delta_callback(pc),a0
                move.l  a0,JOYSTICK_CALLBACK_ENTRY.l
                pea     JOYSTICK_CALLBACK_DESCRIPTOR.l
                moveq   #CALLBACK_DESCRIPTOR_KIND,d0
                move.l  d0,-(a7)
                jsr     REGISTER_CALLBACK_DESCRIPTOR.l
                addq.l  #8,a7
                rts

joystick_delta_callback equ $C1718E
