; Byte-exact target-input continuation $C1B1BE-$C1B1C7.

                org     $C1B1BE

TARGET_INPUT_CONTEXT             equ $C457B4
SHARED_INPUT_CONTEXT_HANDLER     equ $C1C214

route_target_input_to_shared_handler:
                lea.l   TARGET_INPUT_CONTEXT.l,a0
                bra.w   SHARED_INPUT_CONTEXT_HANDLER
