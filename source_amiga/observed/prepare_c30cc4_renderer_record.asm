; Byte-exact observed indexed renderer-record prefix $C30CC4-$C30CD1.
; The nonzero D5 body at $C30CD2 is outside this captured slice.

                org     $C30CC4

RENDERER_POINTER_BLOCK          equ     $C456B6
RENDERER_D5_ZERO_PATH           equ     $C30CD6

prepare_c30cc4_renderer_record:
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  $0(a2,d4.w),d4
                tst.w   d5
                beq.b   RENDERER_D5_ZERO_PATH
