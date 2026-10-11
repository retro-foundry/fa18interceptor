; Byte-exact raw-$24 gear-command slice $C1BC12-$C1BC4F.
; Reached by the sealed run002 documented G event.

                org     $C1BC12

COMMAND_REQUEST_FLAGS          equ $C4599A
GEAR_REQUEST_BIT                equ 0
GEAR_INHIBIT_FLAGS              equ $C46187
GEAR_TOGGLE_FLAGS               equ $C46200
GEAR_TOGGLE_BIT                 equ 7
GEAR_STATUS_WORD                equ $C45B58
GEAR_STATUS_MASK                equ $40
GEAR_MESSAGE_CODE               equ $C45885
GEAR_MESSAGE_VALUE              equ $83

COMMAND_SIDE_EFFECT             equ $C33186
SHARED_COMMAND_FALLBACK         equ $C1C23C

dispatch_gear_command:
                bset.b  #GEAR_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                btst.b  #GEAR_TOGGLE_BIT,GEAR_INHIBIT_FLAGS.l
                bne.s   .submit_side_effect
                bchg.b  #GEAR_TOGGLE_BIT,GEAR_TOGGLE_FLAGS.l
                move.l  GEAR_STATUS_WORD.l,d4
                andi.l  #GEAR_STATUS_MASK,d4
                beq.s   .submit_side_effect
                move.b  #GEAR_MESSAGE_VALUE,GEAR_MESSAGE_CODE.l
                bra.w   SHARED_COMMAND_FALLBACK
.submit_side_effect:
                jsr     COMMAND_SIDE_EFFECT.l
                bra.w   SHARED_COMMAND_FALLBACK
