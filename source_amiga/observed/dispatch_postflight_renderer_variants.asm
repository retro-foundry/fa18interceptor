; Byte-exact static-only postflight renderer dispatcher $C31226-$C31289.

                org     $C31226

POSTFLIGHT_RETURN                equ $C31224
POSTFLIGHT_MASK_LONG             equ $C456E6
POSTFLIGHT_LONG_OFFSET           equ $C45918
POSTFLIGHT_ACTIVITY_BYTE         equ $C45837
POSTFLIGHT_SELECTOR_WORD         equ $C458DA
POSTFLIGHT_MARKER_A              equ $C4E71C
POSTFLIGHT_MARKER_B              equ $C4E744
POSTFLIGHT_BOUNDS_HELPER         equ $C310E2
POSTFLIGHT_VARIANT_ZERO          equ $C3129A
POSTFLIGHT_VARIANT_EIGHT         equ $C31312
POSTFLIGHT_VARIANT_OTHER         equ $C31392

dispatch_postflight_renderer_variants:
                move.l  #$000FFFFF,POSTFLIGHT_MASK_LONG.l
                move.l  #$16B8,d1
                movea.w #4,a4
                move.w  #8,d7
                add.l   POSTFLIGHT_LONG_OFFSET.l,d1
                jsr     POSTFLIGHT_BOUNDS_HELPER.l
                blt.s   POSTFLIGHT_RETURN
                cmpi.b  #1,POSTFLIGHT_ACTIVITY_BYTE.l
                bgt.s   postflight_dispatch_force_defaults
                beq.s   postflight_dispatch_default_variants
                move.w  POSTFLIGHT_SELECTOR_WORD.l,d0
                andi.w  #$E,d0
                beq.s   POSTFLIGHT_VARIANT_ZERO
                cmpi.w  #8,d0
                beq.w   POSTFLIGHT_VARIANT_EIGHT
                bra.w   POSTFLIGHT_VARIANT_OTHER
postflight_dispatch_force_defaults:
                move.w  #-$1,POSTFLIGHT_MARKER_A.l
                move.w  #-$1,POSTFLIGHT_MARKER_B.l
postflight_dispatch_default_variants:
                bsr.w   POSTFLIGHT_VARIANT_ZERO
                bsr.w   POSTFLIGHT_VARIANT_EIGHT
                rts
