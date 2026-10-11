; Byte-exact observed renderer-mode gate at $C30F78-$C30F89.
; The false path into $C30F8A is not claimed; the taken path joins $C30FB2.

                org     $C30F78

RENDERER_MODE_PREPARE           equ     $C310AA
RENDERER_MODE_FLAG              equ     $C45837
RENDERER_MODE_READY             equ     $C30FB2

gate_c30f78_renderer_mode:
                bsr.w   RENDERER_MODE_PREPARE
                move.w  $C458C4.l,d0
                tst.b   RENDERER_MODE_FLAG.l
                bgt.b   RENDERER_MODE_READY
