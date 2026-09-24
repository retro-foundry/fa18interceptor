; Byte-exact C3444C-C3453F table-indexed bounds, submission, and loop exit.
                org     $C3444C
POSTFLIGHT_BOUNDS_TABLE          equ     $C3458C
POSTFLIGHT_TABLE_OFFSET          equ     $C459AA
POSTFLIGHT_X_MIN                 equ     $C34560
POSTFLIGHT_X_MAX                 equ     $C34570
POSTFLIGHT_Y_MIN                 equ     $C34566
POSTFLIGHT_Y_MAX                 equ     $C34562
RENDERER_X_OFFSET                equ     $C45988
RENDERER_Y_OFFSET                equ     $C458D8
DRAW_BLITTER_LINE                equ     $C2FA7E
POSTFLIGHT_COMPONENT_LOOP        equ     $C342DE
POSTFLIGHT_COMPONENTS_A          equ     $C45936
POSTFLIGHT_COMPONENTS_COPY       equ     $C4593A

submit_postflight_component_bounds:
                lea     POSTFLIGHT_BOUNDS_TABLE.l,a0
                adda.w  POSTFLIGHT_TABLE_OFFSET.l,a0
                add.w   (a0),d0
                bge.b   postflight_bound_x0_low
                clr.w   d0
postflight_bound_x0_low:
                cmpi.w  #$13F,d0
                ble.b   postflight_bound_x0_high
                move.w  #$13F,d0
postflight_bound_x0_high:
                add.w   4(a0),d2
                bge.b   postflight_bound_x1_low
                clr.w   d2
postflight_bound_x1_low:
                cmpi.w  #$13F,d2
                ble.b   postflight_bound_x1_high
                move.w  #$13F,d2
postflight_bound_x1_high:
                add.w   2(a0),d1
                add.w   6(a0),d3
                add.w   RENDERER_Y_OFFSET.l,d1
                add.w   RENDERER_Y_OFFSET.l,d3
                move.w  POSTFLIGHT_X_MIN.l,d4
                addq.w  #1,d4
                add.w   RENDERER_X_OFFSET.l,d4
                cmpi.w  #$13F,d4
                bgt.b   postflight_component_skip_submission
                cmp.w   d4,d0
                bge.b   postflight_bound_x0_min
                cmp.w   d0,d2
                bne.b   postflight_bound_x0_set_min
                tst.w   (a0)
                blt.b   postflight_component_skip_submission
postflight_bound_x0_set_min:
                move.w  d4,d0
postflight_bound_x0_min:
                cmp.w   d4,d2
                bge.b   postflight_bound_x1_min
                move.w  d4,d2
postflight_bound_x1_min:
                move.w  POSTFLIGHT_X_MAX.l,d4
                subq.w  #1,d4
                add.w   RENDERER_X_OFFSET.l,d4
                blt.b   postflight_component_skip_submission
                cmp.w   d4,d0
                ble.b   postflight_bound_x0_max
                cmp.w   d0,d2
                bne.b   postflight_bound_x0_set_max
                tst.w   (a0)
                bge.b   postflight_component_skip_submission
postflight_bound_x0_set_max:
                move.w  d4,d0
postflight_bound_x0_max:
                cmp.w   d4,d2
                ble.b   postflight_bound_x1_max
                move.w  d4,d2
postflight_bound_x1_max:
                move.w  POSTFLIGHT_Y_MIN.l,d4
                add.w   RENDERER_Y_OFFSET.l,d4
                cmp.w   d4,d1
                bge.b   postflight_bound_y0_min
                cmp.w   d1,d3
                bne.b   postflight_bound_y0_set_min
                tst.w   2(a0)
                blt.b   postflight_component_skip_submission
postflight_bound_y0_set_min:
                move.w  d4,d1
postflight_bound_y0_min:
                cmp.w   d4,d3
                bge.b   postflight_bound_y1_min
                move.w  d4,d3
postflight_bound_y1_min:
                move.w  POSTFLIGHT_Y_MAX.l,d4
                add.w   RENDERER_Y_OFFSET.l,d4
                cmp.w   d4,d1
                ble.b   postflight_bound_y0_max
                move.w  d4,d1
postflight_bound_y0_max:
                cmp.w   d4,d3
                ble.b   postflight_bound_y1_max
                move.w  d4,d3
postflight_bound_y1_max:
                jsr     DRAW_BLITTER_LINE.l
postflight_component_skip_submission:
                addq.w  #4,POSTFLIGHT_TABLE_OFFSET.l
                cmpi.w  #$10,POSTFLIGHT_TABLE_OFFSET.l
                blt.w   POSTFLIGHT_COMPONENT_LOOP
                move.l  POSTFLIGHT_COMPONENTS_A.l,POSTFLIGHT_COMPONENTS_COPY.l
                move.w  #-$1,POSTFLIGHT_COMPONENTS_A.l
                rts
