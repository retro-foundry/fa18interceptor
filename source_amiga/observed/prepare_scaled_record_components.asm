; Byte-exact component preparation helper $C1C2C8-$C1C40B.
; The caller-visible role is structural: it combines the active record's three
; fields with C45C32-C45C3A, bounds them through C1D974, and publishes C45A62.

                org     $C1C2C8

COMPONENT_PREPARE_ENABLE        equ     $C457AC
ACTIVE_RECORD_BASE              equ     $C46184
ACTIVE_RECORD_OFFSET             equ     $C458DE
ACTIVE_RECORD_COMPONENT_X       equ     $14
ACTIVE_RECORD_COMPONENT_Y       equ     $18
ACTIVE_RECORD_COMPONENT_Z       equ     $1C
COMPONENT_MASK                  equ     $003FFFFF
COMPONENT_ORIGIN_X              equ     $C45C32
COMPONENT_ORIGIN_Y              equ     $C45C36
COMPONENT_ORIGIN_Z              equ     $C45C3A
PREPARED_COMPONENT_X            equ     $C45A62
PREPARED_COMPONENT_Y            equ     $C45A66
PREPARED_COMPONENT_Z            equ     $C45A6A
COMPONENT_SHIFT                 equ     8
SCALE_BOUND_HELPER              equ     $C1D974

prepare_scaled_record_components:
                movem.l d0-d6,-(a7)
                tst.b   COMPONENT_PREPARE_ENABLE.l
                beq.b   prepare_scaled_components_clear
                clr.w   d0
                lea     ACTIVE_RECORD_BASE.l,a0
                dc.w    $D0F9,$00C4,$58DE     ; adda.w ACTIVE_RECORD_OFFSET.l,a0
                move.l  ACTIVE_RECORD_COMPONENT_X(a0),d2
                andi.l  #COMPONENT_MASK,d2
                add.l   COMPONENT_ORIGIN_X.l,d2
                bge.b   prepare_scaled_component_x_nonnegative
                neg.l   d2
                bset    #0,d0
prepare_scaled_component_x_nonnegative:
                asr.l   #COMPONENT_SHIFT,d2
                move.l  ACTIVE_RECORD_COMPONENT_Y(a0),d3
                add.l   COMPONENT_ORIGIN_Y.l,d3
                bge.b   prepare_scaled_component_y_nonnegative
                neg.l   d3
                bset    #1,d0
prepare_scaled_component_y_nonnegative:
                asr.l   #COMPONENT_SHIFT,d3
                move.l  ACTIVE_RECORD_COMPONENT_Z(a0),d4
                andi.l  #COMPONENT_MASK,d4
                add.l   COMPONENT_ORIGIN_Z.l,d4
                bge.b   prepare_scaled_component_z_nonnegative
                neg.l   d4
                bset    #2,d0
prepare_scaled_component_z_nonnegative:
                asr.l   #COMPONENT_SHIFT,d4
                movem.w d2-d4,-(a7)
                jsr     SCALE_BOUND_HELPER.l
                movem.w (a7)+,d2-d4
                move.w  d1,d5
                subi.w  #$0200,d1
                bgt.b   prepare_scaled_components_near_bound
prepare_scaled_components_clear:
                dc.w    $4282                   ; clr.l d2
                dc.w    $4283                   ; clr.l d3
                dc.w    $4284                   ; clr.l d4
                bra.w   prepare_scaled_components_add_origin
prepare_scaled_components_near_bound:
                move.w  d5,d6
                asr.w   #1,d6
                cmp.w   d6,d1
                blt.b   prepare_scaled_components_far_bound
                moveq   #0,d6
                move.w  d5,d6
                sub.w   d1,d6
                ext.l   d6
                swap    d6
                divu.w  d5,d6
                mulu.w  d6,d2
                mulu.w  d6,d3
                mulu.w  d6,d4
                asr.l   #COMPONENT_SHIFT,d2
                bcc.b   prepare_scaled_component_x_round
                addq.l  #1,d2
prepare_scaled_component_x_round:
                asr.l   #COMPONENT_SHIFT,d3
                bcc.b   prepare_scaled_component_y_round
                addq.l  #1,d3
prepare_scaled_component_y_round:
                asr.l   #COMPONENT_SHIFT,d4
                bcc.b   prepare_scaled_component_z_round
                addq.l  #1,d4
prepare_scaled_component_z_round:
                btst    #0,d0
                beq.b   prepare_scaled_component_x_sign_restored
                neg.l   d2
prepare_scaled_component_x_sign_restored:
                btst    #1,d0
                beq.b   prepare_scaled_component_y_sign_restored
                neg.l   d3
prepare_scaled_component_y_sign_restored:
                btst    #2,d0
                beq.b   prepare_scaled_component_z_sign_restored
                neg.l   d4
prepare_scaled_component_z_sign_restored:
                lea     ACTIVE_RECORD_BASE.l,a0
                dc.w    $D0F9,$00C4,$58DE     ; adda.w ACTIVE_RECORD_OFFSET.l,a0
                move.l  ACTIVE_RECORD_COMPONENT_X(a0),d0
                andi.l  #COMPONENT_MASK,d0
                sub.l   d0,d2
                sub.l   ACTIVE_RECORD_COMPONENT_Y(a0),d3
                move.l  ACTIVE_RECORD_COMPONENT_Z(a0),d0
                andi.l  #COMPONENT_MASK,d0
                sub.l   d0,d4
                bra.b   prepare_scaled_components_publish
prepare_scaled_components_far_bound:
                ext.l   d1
                asl.l   #COMPONENT_SHIFT,d1
                divu.w  d5,d1
                mulu.w  d1,d2
                mulu.w  d1,d3
                mulu.w  d1,d4
                btst    #0,d0
                bne.b   prepare_scaled_component_x_far_sign_ready
                neg.l   d2
prepare_scaled_component_x_far_sign_ready:
                btst    #1,d0
                bne.b   prepare_scaled_component_y_far_sign_ready
                neg.l   d3
prepare_scaled_component_y_far_sign_ready:
                btst    #2,d0
                bne.b   prepare_scaled_components_add_origin
                neg.l   d4
prepare_scaled_components_add_origin:
                move.l  COMPONENT_ORIGIN_X.l,d0
                move.l  COMPONENT_ORIGIN_Y.l,d1
                move.l  COMPONENT_ORIGIN_Z.l,d5
                add.l   d0,d2
                add.l   d1,d3
                add.l   d5,d4
prepare_scaled_components_publish:
                move.l  d2,PREPARED_COMPONENT_X.l
                move.l  d3,PREPARED_COMPONENT_Y.l
                move.l  d4,PREPARED_COMPONENT_Z.l
                movem.l (a7)+,d0-d6
                rts
