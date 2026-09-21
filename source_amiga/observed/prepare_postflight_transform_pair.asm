; Byte-exact runtime-observed transform-pair prefix $C32B00-$C32B3F.

                org     $C32B00

POSTFLIGHT_RENDERER_POINTERS    equ $C456B6
POSTFLIGHT_TRANSFORM_REJECT     equ $C32BCC

prepare_postflight_transform_pair:
                adda.l  d7,a4
                move.w  #$142,d7
                movea.l POSTFLIGHT_RENDERER_POINTERS.l,a5
                add.w   d6,d6
                move.w  (a1)+,d5
                move.w  (a1)+,d3
                move.b  (a2)+,d4
                cmpi.b  #$20,d4
                beq.w   POSTFLIGHT_TRANSFORM_REJECT
                swap    d0
                move.w  d0,d2
                swap    d0
                add.w   d6,d2
                add.w   d5,d2
                blt.w   POSTFLIGHT_TRANSFORM_REJECT
                cmpi.w  #$28,d2
                bge.w   POSTFLIGHT_TRANSFORM_REJECT
                add.w   d6,d5
                ext.l   d5
                add.l   a4,d5
                andi.w  #$FF,d4
                subi.w  #$20,d4
