; Byte-exact observed map polygon pre-transform detail-field consumers
; $C2AE5A-$C2AEF7.  The fields are initialized by $C2AD00.

                org     $C2AE5A

apply_map_control_detail_fields:
                clr.w   d4
                tst.w   -$3e(a6)
                bne.b   .coordinates_ready
                asl.l   #$4,d3
                asl.l   #$4,d4
.coordinates_ready:
                add.l   d3,d0
                add.l   d4,d1
                tst.b   -$24(a6)
                bne.b   .force_visible
                tst.w   -$22(a6)
                beq.b   .not_visible
                moveq   #0,d6
                move.l  -$28(a6),d3
                asr.l   #$8,d3
                cmpi.w  #$11,d3
                ble.b   .lookup_limit_ready
                move.w  #$11,d3
.lookup_limit_ready:
                lea     $C2ADF8(pc),a2
                add.w   d3,d3
                move.w  (a2,d3.w),d6
                tst.b   $C457DD.l
                bne.b   .limit_ready
                move.l  #$8000,d7
                cmpi.w  #2,$C45A42.l
                bge.b   .divide_limit
                divu.w  #2,d7
                bra.b   .scale_limit
.divide_limit:
                divu.w  $C45A42.l,d7
.scale_limit:
                mulu.w  d7,d6
                asr.l   #8,d6
.limit_ready:
                swap    d6
                move.l  d0,d7
                addi.l  #$800000,d7
                bge.b   .x_magnitude_ready
                neg.l   d7
.x_magnitude_ready:
                cmp.l   d6,d7
                bgt.b   .visibility_ready
                move.l  d1,d7
                addi.l  #$800000,d7
                bge.b   .y_magnitude_ready
                neg.l   d7
.y_magnitude_ready:
                cmp.l   d6,d7
                ble.b   .not_visible
.force_visible:
                move.w  #1,d7
                bra.b   .visibility_ready
.not_visible:
                clr.w   d7
.visibility_ready:
                tst.w   -$3e(a6)
                beq.b   .swap_components
                swap    d0
                swap    d1
                rol.l   #4,d0
                rol.l   #4,d1
                bra.b   .done
.swap_components:
                swap    d0
                swap    d1
.done:
