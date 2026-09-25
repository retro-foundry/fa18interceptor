; Byte-exact observed renderer D7 gate $C30CD6-$C30CD9.
; The nonzero D7 body at $C30CDA is outside this captured slice.

                org     $C30CD6

RENDERER_D7_ZERO_PATH            equ     $C30CDE

gate_c30cd6_renderer_d7:
                tst.w   d7
                beq.b   RENDERER_D7_ZERO_PATH
