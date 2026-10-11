; Byte-exact fallback raw-key dispatch $C1B010-$C1B037.

                org     $C1B010

RAW_KEY_FALLBACK_A              equ $1B
RAW_KEY_FALLBACK_B              equ $1A
RAW_KEY_RELEASE_A               equ $9B
RAW_KEY_RELEASE_B               equ $9A
RAW_KEY_CONTEXT_MODE            equ $19

DISPATCH_FALLBACK_A             equ $C1BAE2
DISPATCH_FALLBACK_B             equ $C1BB02
QUEUE_FIRE_REQUEST_ACTION       equ $C1BB66
DISPATCH_CONTEXT_MODE           equ $C1C06E
DISPATCH_NONZERO_CONTEXT        equ $C1B038

dispatch_fallback_command_keys:
                cmpi.b  #RAW_KEY_FALLBACK_A,d0
                beq.w   DISPATCH_FALLBACK_A
                cmpi.b  #RAW_KEY_FALLBACK_B,d0
                beq.w   DISPATCH_FALLBACK_B
                cmpi.b  #RAW_KEY_RELEASE_A,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
                cmpi.b  #RAW_KEY_RELEASE_B,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
check_context_mode_key:
                cmpi.b  #RAW_KEY_CONTEXT_MODE,d0
                beq.w   DISPATCH_CONTEXT_MODE

dispatch_nonzero_context:
