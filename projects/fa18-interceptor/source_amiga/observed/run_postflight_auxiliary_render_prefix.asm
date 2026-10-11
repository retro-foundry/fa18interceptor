; Byte-exact C34146-C341B1 first render phase of the postflight auxiliary packet.
                org     $C34146
RENDERER_STATE_LONG             equ     $C456E6
RENDERER_STRIDE                 equ     $C45954
RENDERER_X_OFFSET               equ     $C45988
RENDERER_Y_OFFSET               equ     $C458D8
SUBMIT_ADJUSTED_RENDERER        equ     $C2F5C0
SUBMIT_SHIFTED_RENDERER         equ     $C2F5D4
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
DRAW_BLITTER_LINE               equ     $C2FA7E
POSTFLIGHT_AUXILIARY_NEXT       equ     $C341B2

run_postflight_auxiliary_render_prefix:
                move.l  #$000FFFFF,RENDERER_STATE_LONG.l
                move.w  #10,RENDERER_STRIDE.l
                move.w  #$9F,d0
                move.w  #$47,d1
                jsr     SUBMIT_ADJUSTED_RENDERER.l
                blt.b   POSTFLIGHT_AUXILIARY_NEXT
                addq.w  #1,d1
                jsr     SUBMIT_SHIFTED_RENDERER.l
                addq.w  #2,d1
                jsr     SUBMIT_SHIFTED_RENDERER.l
                addq.w  #1,d1
                jsr     SUBMIT_SHARED_RENDERER.l
                move.w  #$9D,d0
                add.w   RENDERER_X_OFFSET.l,d0
                cmpi.w  #4,d0
                blt.b   POSTFLIGHT_AUXILIARY_NEXT
                move.w  #$48,d1
                move.w  d0,d2
                addq.w  #4,d2
                cmpi.w  #$13B,d2
                bgt.b   POSTFLIGHT_AUXILIARY_NEXT
                move.w  d1,d3
                add.w   RENDERER_Y_OFFSET.l,d1
                add.w   RENDERER_Y_OFFSET.l,d3
                jsr     DRAW_BLITTER_LINE.l
