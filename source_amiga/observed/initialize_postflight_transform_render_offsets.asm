; Byte-exact transform renderer-offset entry $C32A96-$C32AA3.

                org     $C32A96

POSTFLIGHT_RENDER_LANE           equ     $C45986
POSTFLIGHT_RENDER_OFFSET         equ     $C45918
POSTFLIGHT_TRANSFORM_VALUE_TAIL  equ     $C32ACC

initialize_postflight_transform_render_offsets:
                move.w  POSTFLIGHT_RENDER_LANE.l,d6
                move.l  POSTFLIGHT_RENDER_OFFSET.l,d7
                bra.b   POSTFLIGHT_TRANSFORM_VALUE_TAIL

