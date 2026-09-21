; Byte-exact runtime-observed adjusted renderer wrapper $C2F5C0-$C2F5F3.

                org     $C2F5C0

RENDERER_X_ADJUSTMENT           equ $C45988
RENDERER_Y_ADJUSTMENT           equ $C458D8
RENDERER_POINTER_BLOCK          equ $C456B6
RENDERER_WORD_TABLE             equ $C2F766
RENDERER_DISPATCH_TABLE         equ $C2F786
RENDERER_X_LIMIT                equ $0140
RENDERER_NONPOSITIVE_EXIT       equ $C2F622
RENDERER_SHARED_BODY            equ $C2F688
BSR_W_SHARED_BODY_OPCODE        equ $6100
BSR_W_SHARED_BODY_DISPLACEMENT  equ $009C

submit_adjusted_renderer_values:
                add.w   RENDERER_X_ADJUSTMENT.l,d0
                blt.s   RENDERER_NONPOSITIVE_EXIT
                cmpi.w  #RENDERER_X_LIMIT,d0
                bge.s   RENDERER_NONPOSITIVE_EXIT
                add.w   RENDERER_Y_ADJUSTMENT.l,d1
                movea.l RENDERER_POINTER_BLOCK.l,a1
                lea.l   RENDERER_WORD_TABLE.l,a3
                lea.l   RENDERER_DISPATCH_TABLE.l,a4
                move.w  d0,-(a7)
                move.w  d1,-(a7)
                ; VASM relaxes this same-file BSR; retain its original BSR.W.
                dc.w    BSR_W_SHARED_BODY_OPCODE,BSR_W_SHARED_BODY_DISPLACEMENT
                move.w  (a7)+,d1
                move.w  (a7)+,d0
                rts
