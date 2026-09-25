; Byte-exact renderer-pass bound initialization $C2F490-$C2F49B.

                org     $C2F490

RENDERER_PASS_BOUND             equ     $C456E6

initialize_c2f490_renderer_bound:
                move.l  #$000fffff,RENDERER_PASS_BOUND.l
                rts
