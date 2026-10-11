; Byte-exact C34344-C34407 scaled signed normalization of two component deltas.
                org     $C34344
normalize_postflight_component_pair:
                move.w  d2,d4
                sub.w   d0,d2
                bge.b   postflight_component_x_positive
                cmpi.w  #-$F,d2
                blt.b   postflight_component_x_negative_large
                asr.w   d7,d2
                cmpi.w  #-$5,d2
                blt.b   postflight_component_x_negative_scaled
                addq.w  #1,d5
                bra.b   postflight_component_x_negative_scaled
postflight_component_x_negative_large:
                asr.w   d6,d2
postflight_component_x_negative_scaled:
                cmpi.w  #1,d7
                beq.b   postflight_component_x_negative_small_scale
                cmpi.w  #-$1,d2
                bge.b   postflight_component_x_done
                moveq   #-$1,d2
                bra.b   postflight_component_x_done
postflight_component_x_negative_small_scale:
                cmpi.w  #-$2,d2
                bge.b   postflight_component_x_done
                moveq   #-$2,d2
                bra.b   postflight_component_x_done
postflight_component_x_positive:
                cmpi.w  #$F,d2
                bgt.b   postflight_component_x_positive_large
                asr.w   d7,d2
                cmpi.w  #5,d2
                bgt.b   postflight_component_x_positive_scaled
                addq.w  #1,d5
                bra.b   postflight_component_x_positive_scaled
postflight_component_x_positive_large:
                asr.w   d6,d2
postflight_component_x_positive_scaled:
                cmpi.w  #1,d7
                beq.b   postflight_component_x_positive_small_scale
                cmpi.w  #1,d2
                ble.b   postflight_component_x_done
                moveq   #1,d2
                bra.b   postflight_component_x_done
postflight_component_x_positive_small_scale:
                cmpi.w  #2,d2
                ble.b   postflight_component_x_done
                moveq   #2,d2
postflight_component_x_done:
                sub.w   d2,d4
                move.w  d3,d2
                sub.w   d1,d3
                bge.b   postflight_component_y_positive
                cmpi.w  #-$F,d3
                blt.b   postflight_component_y_negative_large
                asr.w   d7,d3
                cmpi.w  #-$5,d3
                blt.b   postflight_component_y_negative_scaled
                addq.w  #1,d5
                bra.b   postflight_component_y_negative_scaled
postflight_component_y_negative_large:
                asr.w   d6,d3
postflight_component_y_negative_scaled:
                cmpi.w  #1,d7
                beq.b   postflight_component_y_negative_small_scale
                cmpi.w  #-$1,d3
                bge.b   postflight_component_y_done
                moveq   #-$1,d3
                bra.b   postflight_component_y_done
postflight_component_y_negative_small_scale:
                cmpi.w  #-$2,d3
                bge.b   postflight_component_y_done
                moveq   #-$2,d3
                bra.b   postflight_component_y_done
postflight_component_y_positive:
                cmpi.w  #$F,d3
                bgt.b   postflight_component_y_positive_large
                asr.w   d7,d3
                cmpi.w  #5,d3
                bgt.b   postflight_component_y_positive_scaled
                addq.w  #1,d5
                bra.b   postflight_component_y_positive_scaled
postflight_component_y_positive_large:
                asr.w   d6,d3
postflight_component_y_positive_scaled:
                cmpi.w  #1,d7
                beq.b   postflight_component_y_positive_small_scale
                cmpi.w  #1,d3
                ble.b   postflight_component_y_done
                moveq   #1,d3
                bra.b   postflight_component_y_done
postflight_component_y_positive_small_scale:
                cmpi.w  #2,d3
                ble.b   postflight_component_y_done
                moveq   #2,d3
postflight_component_y_done:
                sub.w   d3,d2
