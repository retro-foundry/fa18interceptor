; Byte-exact renderer state initializer $C2F490-$C2F49B.

                org     $C2F490

RENDERER_STATE_LONG             equ $C456E6
RENDERER_INITIAL_VALUE          equ $000FFFFF

initialize_renderer_state_long:
                move.l  #RENDERER_INITIAL_VALUE,RENDERER_STATE_LONG.l
                rts
