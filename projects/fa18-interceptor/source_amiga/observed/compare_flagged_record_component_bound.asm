; Byte-exact $C1FC42-$C1FCDD record-component comparison helper.
; Callers consume its condition codes as well as its D7 result.

                org     $C1FC42

RECORD_COMPONENT_BASE           equ     $C45A32
NEGATED_COMPONENT_X             equ     $C45A72
NEGATED_COMPONENT_Y             equ     $C45A76
NEGATED_COMPONENT_Z             equ     $C45A78
RECORD_COMPONENT_X              equ     $C45B2A
RECORD_COMPONENT_Y              equ     $C45B2E
RECORD_COMPONENT_SHIFT          equ     $C45AB8

compare_flagged_record_component_bound:
                movea.l RECORD_COMPONENT_BASE.l,a0
                move.w  d7,d1
                andi.w  #$0C00,d7
                asr.w   #8,d7
                asr.w   #2,d7
                subq.w  #1,d7
                beq.b   .compare_long_component
                subq.w  #1,d7
                beq.b   .compare_y_component
.compare_x_component:
                move.b  $6(a0),d5
                andi.w  #$000F,d5
                move.w  $E(a0,d0.w),d2
                asr.w   d5,d2
                move.w  NEGATED_COMPONENT_Y.l,d7
                neg.w   d7
                move.w  RECORD_COMPONENT_Y.l,d6
                move.w  RECORD_COMPONENT_SHIFT.l,d5
                asl.w   d5,d6
                add.w   d6,d2
                cmp.w   d2,d7
                bra.b   .finish_compare
.compare_y_component:
                move.b  $6(a0),d5
                andi.w  #$000F,d5
                move.w  $A(a0,d0.w),d2
                asr.w   d5,d2
                move.w  NEGATED_COMPONENT_X.l,d7
                neg.w   d7
                move.w  RECORD_COMPONENT_X.l,d6
                move.w  RECORD_COMPONENT_SHIFT.l,d5
                asl.w   d5,d6
                add.w   d6,d2
                cmp.w   d2,d7
                bra.b   .finish_compare
.compare_long_component:
                move.b  $6(a0),d5
                andi.w  #$000F,d5
                move.w  $C(a0,d0.w),d2
                asr.w   d5,d2
                move.l  NEGATED_COMPONENT_Z.l,d7
                neg.l   d7
                ext.l   d2
                cmp.l   d2,d7
.finish_compare:
                blt.b   .below_bound
                btst    #$C,d1
                rts
.below_bound:
                btst    #$C,d1
                bne.b   .clear_result
                moveq   #1,d7
                rts
.clear_result:
                clr.w   d7
                rts
