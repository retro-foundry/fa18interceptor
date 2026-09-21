; Byte-exact raw-$14 target-command slice $C1B1A4-$C1B1BD.
; Reached by the sealed run003 documented T event.

                org     $C1B1A4

COMMAND_SIDE_EFFECT             equ $C33186
COMMAND_REQUEST_FLAGS           equ $C4599A
NEXT_TARGET_REQUEST_BIT          equ 7
TARGET_SELECTION_PENDING         equ $C457B9
SHARED_COMMAND_FALLBACK          equ $C1C23C

request_next_target:
                jsr     COMMAND_SIDE_EFFECT.l
                bset.b  #NEXT_TARGET_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                move.b  #1,TARGET_SELECTION_PENDING.l
                bra.w   SHARED_COMMAND_FALLBACK
