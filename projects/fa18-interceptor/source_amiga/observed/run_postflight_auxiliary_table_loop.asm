; Byte-exact C3421E-C342CF table-driven tail of the postflight auxiliary packet.
                org     $C3421E
POSTFLIGHT_OBJECT_BASE           equ     $C46184
POSTFLIGHT_OBJECT_OFFSET         equ     $C458DE
POSTFLIGHT_TABLE_OFFSET          equ     $C459AA
POSTFLIGHT_VALUE_TABLE           equ     $C34560
POSTFLIGHT_STRIDE_TABLE          equ     $C34578
RENDERER_STRIDE                  equ     $C45954
RENDERER_X_OFFSET                equ     $C45988
RENDERER_Y_OFFSET                equ     $C458D8
DRAW_BLITTER_LINE                equ     $C2FA7E

run_postflight_auxiliary_table_loop:
                lea     POSTFLIGHT_OBJECT_BASE.l,a0
                adda.w  POSTFLIGHT_OBJECT_OFFSET.l,a0
                move.b  $62(a0),d0
                cmpi.b  #$10,d0
                beq.b   postflight_auxiliary_table_begin
                cmpi.b  #$11,d0
                beq.b   postflight_auxiliary_table_begin
                cmpi.b  #$12,d0
                bne.w   postflight_auxiliary_table_done
postflight_auxiliary_table_begin:
                clr.w   POSTFLIGHT_TABLE_OFFSET.l
postflight_auxiliary_table_loop:
                lea     POSTFLIGHT_VALUE_TABLE.l,a0
                adda.w  POSTFLIGHT_TABLE_OFFSET.l,a0
                movem.w (a0),d0-d3
                lea     POSTFLIGHT_STRIDE_TABLE.l,a0
                adda.w  POSTFLIGHT_TABLE_OFFSET.l,a0
                move.w  (a0),RENDERER_STRIDE.l
                add.w   RENDERER_X_OFFSET.l,d0
                bge.b   postflight_auxiliary_left_nonnegative
                clr.w   d0
                add.w   RENDERER_X_OFFSET.l,d2
                blt.b   postflight_auxiliary_skip_line
                bra.b   postflight_auxiliary_clamp_right
postflight_auxiliary_left_nonnegative:
                cmpi.w  #$13F,d0
                ble.b   postflight_auxiliary_right_from_left
                move.w  #$13F,d0
                add.w   RENDERER_X_OFFSET.l,d2
                cmpi.w  #$13F,d2
                ble.b   postflight_auxiliary_submit
                bra.b   postflight_auxiliary_skip_line
postflight_auxiliary_right_from_left:
                add.w   RENDERER_X_OFFSET.l,d2
                bge.b   postflight_auxiliary_clamp_right
                clr.w   d2
postflight_auxiliary_clamp_right:
                cmpi.w  #$13F,d2
                ble.b   postflight_auxiliary_submit
                move.w  #$13F,d2
postflight_auxiliary_submit:
                add.w   RENDERER_Y_OFFSET.l,d1
                add.w   RENDERER_Y_OFFSET.l,d3
                jsr     DRAW_BLITTER_LINE.l
postflight_auxiliary_skip_line:
                addq.w  #4,POSTFLIGHT_TABLE_OFFSET.l
                cmpi.w  #$14,POSTFLIGHT_TABLE_OFFSET.l
                blt.w   postflight_auxiliary_table_loop
postflight_auxiliary_table_done:
                rts
