; Byte-exact nonzero-context dispatch tail $C1B0F2-$C1B125.

                org     $C1B0F2

DISPATCH_MODE                   equ $C457E0
DISPATCH_MODE_TWO               equ 2
RAW_KEY_MODE_A                  equ $31
RAW_KEY_MODE_B                  equ $46
RAW_KEY_SIGNED_STATE            equ $45
RAW_KEY_RETURN                  equ $44
RETURN_CONTEXT_STATE            equ $C457D3

FINISH_INPUT_EVENT              equ $C1C2B8
SET_INPUT_STATE_SIGN_FLAG       equ $C1C224
SHARED_COMMAND_QUEUE            equ $C1C23C

finish_nonzero_context_key_dispatch:
                cmpi.b  #DISPATCH_MODE_TWO,DISPATCH_MODE.l
                beq.b   .check_mode_b
                cmpi.b  #RAW_KEY_MODE_A,d0
                beq.w   FINISH_INPUT_EVENT
.check_mode_b:
                cmpi.b  #RAW_KEY_MODE_B,d0
                beq.w   FINISH_INPUT_EVENT
                cmpi.b  #RAW_KEY_SIGNED_STATE,d0
                beq.w   SET_INPUT_STATE_SIGN_FLAG
                cmpi.b  #RAW_KEY_RETURN,d0
                bne.w   SHARED_COMMAND_QUEUE
                clr.b   RETURN_CONTEXT_STATE.l
                bra.w   SHARED_COMMAND_QUEUE
