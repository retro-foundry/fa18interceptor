; Byte-exact partially runtime-observed remaining transform lanes $C32B90-$C32BD1.
; Run024 reaches the preceding second-lane return; lanes at offsets 8 and 12
; and the pair-loop back edge are retained from static byte evidence.

                org     $C32B90

POSTFLIGHT_LANE_ENABLE_BITS     equ $C45955
POSTFLIGHT_MASK_UPDATE          equ $C330FE
POSTFLIGHT_LANE_STANDARD        equ $0B0A
POSTFLIGHT_LANE_ENABLED         equ $0BFA
POSTFLIGHT_TRANSFORM_PAIR       equ $C32B0E

submit_remaining_postflight_transform_lanes:
                move.l  8(a5),d1
                add.l   d5,d1
                move.w  #POSTFLIGHT_LANE_STANDARD,d2
                btst    #1,POSTFLIGHT_LANE_ENABLE_BITS.l
                beq.b   .third_mask_ready
                move.w  #POSTFLIGHT_LANE_ENABLED,d2
.third_mask_ready:
                or.w    d3,d2
                bsr.w   POSTFLIGHT_MASK_UPDATE
                move.l  12(a5),d1
                add.l   d5,d1
                move.w  #POSTFLIGHT_LANE_STANDARD,d2
                btst    #0,POSTFLIGHT_LANE_ENABLE_BITS.l
                beq.b   .fourth_mask_ready
                move.w  #POSTFLIGHT_LANE_ENABLED,d2
.fourth_mask_ready:
                or.w    d3,d2
                bsr.w   POSTFLIGHT_MASK_UPDATE
                dbra    d0,POSTFLIGHT_TRANSFORM_PAIR
                rts
