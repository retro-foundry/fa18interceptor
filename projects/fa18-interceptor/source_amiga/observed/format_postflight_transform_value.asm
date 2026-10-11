; Byte-exact runtime-observed transform value formatter $C32AD0-$C32AFF.

                org     $C32AD0

POSTFLIGHT_FORMAT_SOURCE        equ $C45B22
POSTFLIGHT_TRANSFORM_PAIR_LOOP  equ $C32B00

format_postflight_transform_value:
                move.l  POSTFLIGHT_FORMAT_SOURCE.l,d3
                move.w  d2,d5
format_postflight_transform_nibble:
                move.w  d3,d1
                andi.w  #$F,d1
                addi.w  #$30,d1
                move.b  d1,-(a0)
                lsr.l   #4,d3
                dbra    d2,format_postflight_transform_nibble
                tst.b   d4
                bne.b   POSTFLIGHT_TRANSFORM_PAIR_LOOP
                subq.w  #1,d5
format_postflight_transform_blanks:
                cmpi.b  #$30,(a0)+
                bne.b   POSTFLIGHT_TRANSFORM_PAIR_LOOP
                move.b  #$20,-1(a0)
                subq.w  #1,d5
                bge.b   format_postflight_transform_blanks
