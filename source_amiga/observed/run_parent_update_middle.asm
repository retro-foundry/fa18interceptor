; Byte-exact parent-update middle slice $C0F01C-$C0F08F.
; Runs after C1C63E and before the later flight-update conditional.

                org     $C0F01C

UPDATE_STAGE_MARKER             equ $C45AD4
MARKER_MATRIX_PIPELINE          equ $20
MARKER_POST_OCTANT              equ $60
MARKER_CONDITIONAL_STAGE        equ $68
MARKER_LATER_STAGE              equ $70
CONDITIONAL_STAGE_FLAG          equ $C4589B
CONDITIONAL_STAGE_INHIBIT       equ $C457B0

POST_C1C63E_STAGE               equ $C25B1C
MATRIX_PIPELINE                 equ $C2D99C
POST_MATRIX_STAGE               equ $C1C54E
ANGLE_OCTANT_STAGE              equ $C254E8
POST_OCTANT_STAGE               equ $C122A2
POST_OCTANT_UPDATE              equ $C1C860
STAGE_MARKER_60_HELPER_1        equ $C2559A
STAGE_MARKER_60_HELPER_2        equ $C25864
STAGE_MARKER_60_HELPER_3        equ $C0D730
CONDITIONAL_STAGE               equ $C2AA9C
LATER_STAGE                     equ $C0DAEE

run_parent_update_middle:
                move.w  #MARKER_MATRIX_PIPELINE,UPDATE_STAGE_MARKER.l
                jsr     POST_C1C63E_STAGE.l
                jsr     MATRIX_PIPELINE.l
                jsr     POST_MATRIX_STAGE.l
                jsr     ANGLE_OCTANT_STAGE.l
                jsr     POST_OCTANT_STAGE.l
                jsr     POST_OCTANT_UPDATE.l
                move.w  #MARKER_POST_OCTANT,UPDATE_STAGE_MARKER.l
                jsr     STAGE_MARKER_60_HELPER_1.l
                jsr     STAGE_MARKER_60_HELPER_2.l
                jsr     STAGE_MARKER_60_HELPER_3.l
                move.w  #MARKER_CONDITIONAL_STAGE,UPDATE_STAGE_MARKER.l
                move.b  CONDITIONAL_STAGE_FLAG.l,d0
                tst.b   d0
                beq.s   .run_conditional_stage
                tst.b   CONDITIONAL_STAGE_INHIBIT.l
                beq.s   .after_conditional_stage
.run_conditional_stage:
                jsr     CONDITIONAL_STAGE.l
.after_conditional_stage:
                move.w  #MARKER_LATER_STAGE,UPDATE_STAGE_MARKER.l
                jsr     LATER_STAGE.l
