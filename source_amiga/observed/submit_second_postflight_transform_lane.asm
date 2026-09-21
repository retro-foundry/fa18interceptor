; Byte-exact runtime-observed second transform-lane submission $C32B72-$C32B8D.

                org     $C32B72

POSTFLIGHT_LANE_ENABLE_BITS     equ $C45955
POSTFLIGHT_MASK_UPDATE          equ $C330FE
POSTFLIGHT_SECOND_LANE_STANDARD equ $0B0A
POSTFLIGHT_SECOND_LANE_ENABLED  equ $0BFA

submit_second_postflight_transform_lane:
                move.l  4(a5),d1
                add.l   d5,d1
                move.w  #POSTFLIGHT_SECOND_LANE_STANDARD,d2
                btst    #2,POSTFLIGHT_LANE_ENABLE_BITS.l
                beq.b   .mask_ready
                move.w  #POSTFLIGHT_SECOND_LANE_ENABLED,d2
.mask_ready:
                or.w    d3,d2
                bsr.w   POSTFLIGHT_MASK_UPDATE
