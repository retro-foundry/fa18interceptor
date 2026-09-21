; Byte-exact static-only renderer-coordinate preparation $C334A2-$C334DF.
                org     $C334A2
POSTFLIGHT_HORIZONTAL_OFFSET equ $C45988
POSTFLIGHT_VERTICAL_OFFSET equ $C458D8
POSTFLIGHT_RESULT_WORD equ $C4598C
POSTFLIGHT_RENDERER equ $C2F5D4
prepare_postflight_renderer_coordinate:
                move.w  #$DF,d0
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                move.w  d1,-(a7)
                addi.w  #$5B,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                move.w  d1,POSTFLIGHT_RESULT_WORD.l
                addq.w  #2,d1
                tst.w   d0
                ble.s   postflight_coordinate_done
                cmpi.w  #$13F,d0
                bge.s   postflight_coordinate_done
                jsr     POSTFLIGHT_RENDERER.l
postflight_coordinate_done:
                move.w  (a7)+,d5
                asl.w   #3,d5
                move.w  d5,d3
                add.w   d3,d3
                add.w   d3,d3
                add.w   d3,d5
                ext.l   d5
