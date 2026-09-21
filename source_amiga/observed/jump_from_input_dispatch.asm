; Byte-exact input-dispatch transfer $C1C2B8-$C1C2BD.

                org     $C1C2B8

CONTINUE_INPUT_DISPATCH         equ $C06BF0

jump_from_input_dispatch:
                jmp     CONTINUE_INPUT_DISPATCH.l
