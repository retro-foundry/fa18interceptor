; Byte-exact runtime-observed postflight transform-submit entry $C32AA4-$C32AB3.
; Run024 frame-23000 continuation reaches this entry from transform helpers.

                org     $C32AA4

POSTFLIGHT_RENDERER_LANE_OFFSET equ $C45986
POSTFLIGHT_LONG_OFFSET          equ $C45918
POSTFLIGHT_TRANSFORM_FORMAT     equ $C32AD0

initialize_postflight_transform_submit:
                moveq   #0,d4
                move.w  POSTFLIGHT_RENDERER_LANE_OFFSET.l,d6
                move.l  POSTFLIGHT_LONG_OFFSET.l,d7
                bra.b   POSTFLIGHT_TRANSFORM_FORMAT
