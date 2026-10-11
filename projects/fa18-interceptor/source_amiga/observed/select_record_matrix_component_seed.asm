; Byte-exact static component-seed selection prefix $C1C54E-$C1C5DF.
; Selects one literal triple from active-record byte +$62, applies the nine
; signed words at +$92..+$A2, then falls through to C1C5E0's publisher.

                org     $C1C54E

COMPONENT_CONTEXT_GUARD          equ     $C45785
ACTIVE_RECORD_BASE                equ     $C46184
ACTIVE_RECORD_OFFSET              equ     $C458DE
ACTIVE_RECORD_TYPE                equ     $62
ACTIVE_RECORD_TYPE_SPECIAL        equ     $30
ACTIVE_RECORD_TYPE_ALTERNATE      equ     $11
ACTIVE_RECORD_MATRIX_00           equ     $92
ACTIVE_RECORD_MATRIX_01           equ     $94
ACTIVE_RECORD_MATRIX_02           equ     $96
ACTIVE_RECORD_MATRIX_10           equ     $98
ACTIVE_RECORD_MATRIX_11           equ     $9A
ACTIVE_RECORD_MATRIX_12           equ     $9C
ACTIVE_RECORD_MATRIX_20           equ     $9E
ACTIVE_RECORD_MATRIX_21           equ     $A0
ACTIVE_RECORD_MATRIX_22           equ     $A2
COMPONENT_MATRIX_SHIFT            equ     6
PUBLISH_COMPONENTS_CONTINUATION   equ     $C1C5E0
CONTEXT_GUARD_CONTINUATION        equ     $C1C60C

select_record_matrix_component_seed:
                tst.b   COMPONENT_CONTEXT_GUARD.l
                bne.w   CONTEXT_GUARD_CONTINUATION
                lea     ACTIVE_RECORD_BASE.l,a2
                dc.w    $D4F9,$00C4,$58DE     ; adda.w ACTIVE_RECORD_OFFSET.l,a2
                cmpi.b  #ACTIVE_RECORD_TYPE_SPECIAL,ACTIVE_RECORD_TYPE(a2)
                bne.b   select_record_matrix_component_seed_other_type
                moveq   #0,d3
                moveq   #1,d4
                moveq   #-5,d5
                bra.b   select_record_matrix_component_seed_ready
select_record_matrix_component_seed_other_type:
                moveq   #0,d3
                cmpi.b  #ACTIVE_RECORD_TYPE_ALTERNATE,ACTIVE_RECORD_TYPE(a2)
                beq.b   select_record_matrix_component_seed_alternate_type
                moveq   #4,d4
                moveq   #$12,d5
                bra.b   select_record_matrix_component_seed_ready
select_record_matrix_component_seed_alternate_type:
                moveq   #5,d4
                moveq   #$14,d5
select_record_matrix_component_seed_ready:
                move.w  d3,d6
                move.w  d4,d0
                move.w  d5,d7
                muls.w  ACTIVE_RECORD_MATRIX_00(a2),d6
                muls.w  ACTIVE_RECORD_MATRIX_01(a2),d0
                muls.w  ACTIVE_RECORD_MATRIX_02(a2),d7
                add.l   d6,d0
                add.l   d7,d0
                move.w  d3,d6
                move.w  d4,d1
                move.w  d5,d7
                muls.w  ACTIVE_RECORD_MATRIX_10(a2),d6
                muls.w  ACTIVE_RECORD_MATRIX_11(a2),d1
                muls.w  ACTIVE_RECORD_MATRIX_12(a2),d7
                add.l   d6,d1
                add.l   d7,d1
                move.w  d4,d2
                muls.w  ACTIVE_RECORD_MATRIX_20(a2),d3
                muls.w  ACTIVE_RECORD_MATRIX_21(a2),d2
                muls.w  ACTIVE_RECORD_MATRIX_22(a2),d5
                add.l   d3,d2
                add.l   d5,d2
                asr.l   #COMPONENT_MATRIX_SHIFT,d0
                asr.l   #COMPONENT_MATRIX_SHIFT,d1
                asr.l   #COMPONENT_MATRIX_SHIFT,d2
                movem.l $14(a2),d3-d5
                add.l   d0,d3
                add.l   d1,d4
                add.l   d2,d5
                movem.l d3-d5,$C45A7C.l
                ; Fall through into PUBLISH_COMPONENTS_CONTINUATION.
