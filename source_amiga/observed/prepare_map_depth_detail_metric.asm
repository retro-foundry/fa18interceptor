; Byte-exact map depth/detail metric preparation $C2AA9C-$C2AB33.
; C45A78 is the current projection-depth longword for this renderer family.

                org     $C2AA9C

prepare_map_depth_detail_metric:
                link.w  a6,#$ffba
                clr.b   $C4589D.l
                clr.w   $C4BF90.l
                tst.b   $C457B0.l
                bne.b   .initial_bounds_ready
                move.w  #2,d0
                move.w  #2,d1
                bra.b   .publish_initial_bounds
.initial_bounds_ready:
                move.w  #$f,d0
                move.w  #$ffff,d1
.publish_initial_bounds:
                clr.w   d2
                clr.w   d3
                movem.w d0-d3,$C456E6.l
                move.l  $C45A78.l,d0
                neg.l   d0
                tst.b   $C457DD.l
                bne.b   .metric_ready
                cmpi.l  #$7fff0,d0
                bgt.b   .metric_ready
                asr.l   #4,d0
                move.l  #$8000,d7
                cmpi.w  #2,$C45A42.l
                bge.b   .divide_metric
                divu.w  #2,d7
                bra.b   .scale_metric
.divide_metric:
                divu.w  $C45A42.l,d7
.scale_metric:
                mulu.w  d7,d0
                asr.l   #4,d0
.metric_ready:
                move.l  d0,-$28(a6)
                cmpi.l  #$3f8,d0
                ble.b   .skip_depth_helper
                move.w  #6,$C45954.l
                bsr.w   $C2AB5A
.skip_depth_helper:
                move.w  #6,$C45954.l
                bsr.w   $C2AB34
                unlk    a6
                rts
