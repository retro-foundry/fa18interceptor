; Byte-exact partially runtime-observed first transform-lane submission $C32B40-$C32B71.
; Run024 takes the normal submission path; its odd-result rejection is unobserved.

                org     $C32B40

POSTFLIGHT_LANE_OFFSET_TABLE    equ $C3D790
POSTFLIGHT_LANE_ENABLE_BITS     equ $C45955
POSTFLIGHT_MASK_UPDATE          equ $C330FE
POSTFLIGHT_FIRST_LANE_STANDARD  equ $0B0A
POSTFLIGHT_FIRST_LANE_ENABLED   equ $0BFA
POSTFLIGHT_TRANSFORM_REJECT     equ $C32BCC

submit_first_postflight_transform_lane:
                add.w   d4,d4
                lea.l   POSTFLIGHT_LANE_OFFSET_TABLE.l,a3
                adda.w  (a3,d4.w),a3
                move.l  a3,d4
                move.l  (a5),d1
                add.l   d5,d1
                btst    #0,d1
                beq.b   .submit
                bra.b   POSTFLIGHT_TRANSFORM_REJECT
.submit:
                move.w  #POSTFLIGHT_FIRST_LANE_STANDARD,d2
                btst    #3,POSTFLIGHT_LANE_ENABLE_BITS.l
                beq.b   .mask_ready
                move.w  #POSTFLIGHT_FIRST_LANE_ENABLED,d2
.mask_ready:
                or.w    d3,d2
                bsr.w   POSTFLIGHT_MASK_UPDATE
