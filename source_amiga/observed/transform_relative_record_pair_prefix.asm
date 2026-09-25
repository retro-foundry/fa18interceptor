; Byte-exact observed transform continuation $C1E8D6-$C1E957.
; This feeds the cross-product/dot-product tail at $C1E95C; record roles are
; still structural outside the proved arithmetic data flow.

                org     $C1E8D6

PREPARED_COMPONENT_X            equ     $C45A72
PREPARED_COMPONENT_Y            equ     $C45A76
PREPARED_COMPONENT_DEPTH        equ     $C45A78

transform_relative_record_pair_prefix:
                move.w  (a4),d1
                andi.w  #$0FFF,d1
                addi.w  #$A4,d0
                addi.w  #$A4,d1
                move.w  #$FF00,d3
                lea     (a2,d0.w),a5
                movem.w (a2,d1.w),d5-d7
                sub.w   (a5),d5
                sub.w   4(a5),d7
                muls.w  d3,d7
                asr.l   #6,d7
                muls.w  d3,d5
                neg.l   d5
                asr.l   #6,d5
                exg.l   d5,d7
                movem.w (a5),d2-d4
                move.w  -$20(a6),d0
                asr.w   d0,d2
                asr.l   d0,d3
                asr.w   d0,d4
                add.w   $c(a2),d2
                add.l   $10(a2),d3
                add.w   $e(a2),d4
                add.w   PREPARED_COMPONENT_X.l,d2
                add.l   PREPARED_COMPONENT_DEPTH.l,d3
                add.w   PREPARED_COMPONENT_Y.l,d4
                neg.w   d2
                neg.w   d3
                neg.w   d4
                muls.w  d2,d5
                muls.w  d4,d7
                add.l   d5,d7
                blt.b   $C1E94E
                tst.w   -$18(a6)
                bne.w   $C1E8B8
                move.w  (a4)+,d0
                bge.b   $C1E946
                bra.w   $C1E8B8
                tst.w   -$18(a6)
                beq.w   $C1E8C4
