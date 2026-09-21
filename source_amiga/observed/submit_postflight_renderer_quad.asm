; Byte-exact static-only postflight renderer sequence $C31312-$C31391.

                org     $C31312

POSTFLIGHT_HORIZONTAL_OFFSET     equ $C45988
POSTFLIGHT_VERTICAL_OFFSET       equ $C458D8
POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_SHARED_RENDERER       equ $C2F5F4
POSTFLIGHT_VARIANT_OTHER         equ $C31392

submit_postflight_renderer_quad:
                move.w  #$9D,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                blt.s   POSTFLIGHT_VARIANT_OTHER
                cmpi.w  #$140,d0
                bge.s   POSTFLIGHT_VARIANT_OTHER
                move.w  #$A8,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                move.w  #$C,POSTFLIGHT_RENDERER_SELECTOR.l
                jsr     POSTFLIGHT_SHARED_RENDERER.l
                move.w  #$9F,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                blt.s   POSTFLIGHT_VARIANT_OTHER
                cmpi.w  #$140,d0
                bge.s   POSTFLIGHT_VARIANT_OTHER
                move.w  #$A8,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                jsr     POSTFLIGHT_SHARED_RENDERER.l
                move.w  #$9E,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                move.w  #$A8,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                jsr     POSTFLIGHT_SHARED_RENDERER.l
                move.w  #$9E,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                move.w  #$A7,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                jsr     POSTFLIGHT_SHARED_RENDERER.l
