; Byte-exact transform-pair renderer-offset entries $C32AB4-$C32ACF.

                org     $C32AB4

POSTFLIGHT_RENDER_LANE           equ     $C45986
POSTFLIGHT_RENDER_OFFSET         equ     $C45918
POSTFLIGHT_TRANSFORM_PAIR        equ     $C32B00
POSTFLIGHT_TRANSFORM_VALUE_TAIL  equ     $C32ACC

initialize_postflight_transform_pair_offsets:
                move.w  POSTFLIGHT_RENDER_LANE.l,d6
                move.l  POSTFLIGHT_RENDER_OFFSET.l,d7
                bra.b   POSTFLIGHT_TRANSFORM_PAIR

initialize_postflight_transform_pair_zero_offsets:
                moveq   #0,d6
                moveq   #0,d7
                bra.b   POSTFLIGHT_TRANSFORM_PAIR

initialize_postflight_transform_value_zero_offsets:
                moveq   #0,d6
                moveq   #0,d7
postflight_transform_value_tail:
                move.w  d0,d2
                moveq   #0,d4

