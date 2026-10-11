; Byte-exact C341B2-C3421D two bounded line stages of the auxiliary packet.
                org     $C341B2
RENDERER_STRIDE                 equ     $C45954
RENDERER_X_OFFSET               equ     $C45988
RENDERER_Y_OFFSET               equ     $C458D8
DRAW_BLITTER_LINE               equ     $C2FA7E
POSTFLIGHT_AUXILIARY_TABLE_LOOP equ     $C3421E

run_postflight_auxiliary_line_stages:
                move.w  #10,RENDERER_STRIDE.l
                move.w  #$8D,d0
                add.w   RENDERER_X_OFFSET.l,d0
                cmpi.w  #5,d0
                blt.b   postflight_auxiliary_line_stage_two
                move.w  #$5A,d1
                move.w  d0,d2
                addq.w  #4,d2
                cmpi.w  #$13B,d2
                bgt.b   postflight_auxiliary_line_stage_two
                move.w  d1,d3
                add.w   RENDERER_Y_OFFSET.l,d1
                add.w   RENDERER_Y_OFFSET.l,d3
                jsr     DRAW_BLITTER_LINE.l
postflight_auxiliary_line_stage_two:
                move.w  #$AD,d0
                add.w   RENDERER_X_OFFSET.l,d0
                cmpi.w  #5,d0
                blt.b   POSTFLIGHT_AUXILIARY_TABLE_LOOP
                move.w  #$5A,d1
                move.w  d0,d2
                addq.w  #4,d2
                cmpi.w  #$13B,d2
                bgt.b   POSTFLIGHT_AUXILIARY_TABLE_LOOP
                move.w  d1,d3
                add.w   RENDERER_Y_OFFSET.l,d1
                add.w   RENDERER_Y_OFFSET.l,d3
                jsr     DRAW_BLITTER_LINE.l
