; Byte-exact raw-$20 arrestor-hook command $C1B616-$C1B663.
; Reached by sealed run003 documented A event.

                org     $C1B616

COMMAND_REQUEST_FLAGS           equ $C4599A
ARRESTOR_HOOK_REQUEST_BIT        equ 1
ARRESTOR_HOOK_CONTEXT            equ $C461E6
ARRESTOR_HOOK_CONTEXT_VALUE      equ $11
COMMAND_SIDE_EFFECT              equ $C33186
ARRESTOR_HOOK_STATE              equ $C46186
ARRESTOR_HOOK_STATE_BIT          equ 15
ARRESTOR_HOOK_STATE_MASK         equ $8000
ARRESTOR_HOOK_STATUS_FLAGS       equ $C45847
ARRESTOR_HOOK_STATUS_BIT         equ 7
ARRESTOR_HOOK_MODE               equ $C45845
ARRESTOR_HOOK_MODE_VALUE         equ 3
ARRESTOR_HOOK_COMMAND_WORD       equ $4023
PUBLISH_COMMAND_WORD_FLAGS       equ $C25704
SHARED_COMMAND_FALLBACK          equ $C1C23C

toggle_arrestor_hook:
                bset.b  #ARRESTOR_HOOK_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                cmpi.b  #ARRESTOR_HOOK_CONTEXT_VALUE,ARRESTOR_HOOK_CONTEXT.l
                bne.w   SHARED_COMMAND_FALLBACK
                jsr     COMMAND_SIDE_EFFECT.l
                eori.w  #ARRESTOR_HOOK_STATE_MASK,ARRESTOR_HOOK_STATE.l
                bchg.b  #ARRESTOR_HOOK_STATUS_BIT,ARRESTOR_HOOK_STATUS_FLAGS.l
                move.b  #ARRESTOR_HOOK_MODE_VALUE,ARRESTOR_HOOK_MODE.l
                swap    d0
                btst.b  #7,ARRESTOR_HOOK_STATE.l
                beq.s   .publish_hook_state
                move.w  #ARRESTOR_HOOK_COMMAND_WORD,d0
.publish_hook_state:
                jsr     PUBLISH_COMMAND_WORD_FLAGS.l
                swap    d0
                bra.w   SHARED_COMMAND_FALLBACK
