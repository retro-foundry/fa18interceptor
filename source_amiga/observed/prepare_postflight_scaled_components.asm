; Byte-exact C342D0-C34343 setup and scale selection for postflight components.
                org     $C342D0
POSTFLIGHT_TABLE_OFFSET          equ     $C459AA
RENDERER_STRIDE                  equ     $C45954
POSTFLIGHT_COMPONENTS_A          equ     $C45936
POSTFLIGHT_COMPONENTS_B          equ     $C45942
POSTFLIGHT_ACTIVITY_BYTE         equ     $C457AE
POSTFLIGHT_OBJECT_BASE           equ     $C46184
POSTFLIGHT_OBJECT_OFFSET         equ     $C458DE
POSTFLIGHT_SCALE_FLAGS           equ     $C45B50
POSTFLIGHT_COMPONENTS_JOIN       equ     $C34408
POSTFLIGHT_ACTIVITY_EXIT         equ     $C34414

prepare_postflight_scaled_components:
                clr.w   POSTFLIGHT_TABLE_OFFSET.l
                move.w  #10,RENDERER_STRIDE.l
                movem.w POSTFLIGHT_COMPONENTS_A.l,d0-d1
                tst.b   POSTFLIGHT_ACTIVITY_BYTE.l
                bne.w   POSTFLIGHT_ACTIVITY_EXIT
                movem.w POSTFLIGHT_COMPONENTS_B.l,d2-d3
                moveq   #0,d5
                lea     POSTFLIGHT_OBJECT_BASE.l,a1
                adda.w  POSTFLIGHT_OBJECT_OFFSET.l,a1
                move.b  $63(a1),d4
                andi.b  #$F0,d4
                beq.b   postflight_component_defaults
                tst.w   d0
                bgt.b   postflight_component_scaling
postflight_component_defaults:
                move.w  #-$1,d4
                bra.w   POSTFLIGHT_COMPONENTS_JOIN
postflight_component_scaling:
                tst.w   d2
                bgt.b   postflight_component_scale_flags
                move.w  #$9F,d4
                move.w  #$5B,d2
                bra.w   POSTFLIGHT_COMPONENTS_JOIN
postflight_component_scale_flags:
                move.l  POSTFLIGHT_SCALE_FLAGS.l,d6
                andi.l  #$4000,d6
                bne.b   postflight_component_large_scale
                moveq   #4,d6
                moveq   #2,d7
                bra.b   postflight_component_scale_ready
postflight_component_large_scale:
                moveq   #2,d6
                moveq   #1,d7
postflight_component_scale_ready:
