; Byte-exact throttle-mode update tail $C1B5DC-$C1B601.
; Entered by the $0D/$41 route after select_throttle_mode.

                org     $C1B5DC

RESET_THROTTLE_INPUT_STATE       equ $C1B602
POST_THROTTLE_UPDATE_HELPER      equ $C33186
STATUS_FLAGS                     equ $C4599B
STATUS_FLAG_0                    equ 0
THROTTLE_UPDATE_MODE             equ $C4583F
THROTTLE_UPDATE_MODE_VALUE       equ 3
CONTROL_RECORD_BASE              equ $C46184
CONTROL_RECORD_WORD_FLAG         equ $0800
SHARED_COMMAND_FALLBACK          equ $C1C23C

finish_throttle_mode_update:
                bsr.w   RESET_THROTTLE_INPUT_STATE
                jsr     POST_THROTTLE_UPDATE_HELPER
                bset.b  #STATUS_FLAG_0,STATUS_FLAGS.l
                move.b  #THROTTLE_UPDATE_MODE_VALUE,THROTTLE_UPDATE_MODE.l
                eori.w  #CONTROL_RECORD_WORD_FLAG,CONTROL_RECORD_BASE.l
                bra.w   SHARED_COMMAND_FALLBACK
