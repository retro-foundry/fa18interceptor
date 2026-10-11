; Byte-exact callback-descriptor removal $C1748C-$C1749F.
; The bounded native-Delete input transition calls this before re-registering
; the same descriptor through $C17456.

                org     $C1748C

JOYSTICK_CALLBACK_DESCRIPTOR       equ $C1ABF0
REMOVE_CALLBACK_DESCRIPTOR         equ $C53B18
CALLBACK_DESCRIPTOR_KIND           equ 5

remove_joystick_delta_callback:
                pea     JOYSTICK_CALLBACK_DESCRIPTOR.l
                moveq   #CALLBACK_DESCRIPTOR_KIND,d0
                move.l  d0,-(a7)
                jsr     REMOVE_CALLBACK_DESCRIPTOR.l
                addq.l  #8,a7
                rts
