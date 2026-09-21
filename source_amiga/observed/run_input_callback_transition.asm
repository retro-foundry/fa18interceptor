; Byte-exact static-only input callback transition $C06BF0-$C06C03.
; Reached by the input dispatcher at $C1C2B8; no available capture executes it.

                org     $C06BF0

INPUT_CALLBACK_TRANSITION_A    equ $C1748C
INPUT_CALLBACK_TRANSITION_B    equ $C17456
INPUT_CALLBACK_RETURN_HELPER    equ $C06C02

run_input_callback_transition:
                jsr     INPUT_CALLBACK_TRANSITION_A.l
                bsr.w   INPUT_CALLBACK_RETURN_HELPER
                jsr     INPUT_CALLBACK_TRANSITION_B.l
                rts

input_callback_transition_return:
                rts
