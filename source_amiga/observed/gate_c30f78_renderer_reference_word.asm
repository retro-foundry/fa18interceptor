; Byte-exact observed renderer-mode continuation $C30F8A-$C30F95.
; It loads a shared renderer reference word, routes negative values to the
; adjacent path, and returns to the prior mode branch on equality with D0.

                org     $C30F8A

RENDERER_REFERENCE_WORD         equ     $C459A2

gate_c30f78_renderer_reference_word:
                move.w  RENDERER_REFERENCE_WORD.l,d2
                blt.b   $C30FA2
                cmp.w   d2,d0
                beq.b   $C30F76
