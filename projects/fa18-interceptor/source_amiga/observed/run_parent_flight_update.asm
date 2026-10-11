; Byte-exact conditional flight-update slice $C0F090-$C0F123.

                org     $C0F090

UPDATE_STAGE_MARKER             equ $C45AD4
STAGE_SIGNED_LONG               equ $C45A66
FLIGHT_UPDATE_SKIP_LIMIT        equ $F8000000
FLIGHT_UPDATE_FLAG              equ $C457AD
MARKER_FLIGHT_UPDATE_1          equ $78
MARKER_FLIGHT_UPDATE_2          equ $80
MARKER_FLIGHT_UPDATE_3          equ $90
MARKER_FLIGHT_DECISION          equ $A0
MARKER_FLIGHT_DECISION_TRUE     equ $A4
MARKER_FLIGHT_FOLLOWUP_TRUE     equ $A8
MARKER_FLIGHT_DECISION_FALSE    equ $AC
MARKER_FLIGHT_FINAL             equ $B0
NEXT_PARENT_STAGE               equ $C0F124

FLIGHT_UPDATE_STAGE             equ $C1CB14
POST_FLIGHT_UPDATE_STAGE        equ $C1CB26
FLIGHT_UPDATE_HELPER            equ $C279D0
FLIGHT_DECISION_HELPER          equ $C265E8
FLIGHT_BRANCH_HELPER            equ $C1518C
FLIGHT_FOLLOWUP_HELPER          equ $C1CCBC

run_parent_flight_update:
                cmpi.l  #FLIGHT_UPDATE_SKIP_LIMIT,STAGE_SIGNED_LONG.l
                ble.w   NEXT_PARENT_STAGE
                move.w  #MARKER_FLIGHT_UPDATE_1,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_UPDATE_STAGE.l
                move.w  #MARKER_FLIGHT_UPDATE_2,UPDATE_STAGE_MARKER.l
                jsr     POST_FLIGHT_UPDATE_STAGE.l
                move.w  #MARKER_FLIGHT_UPDATE_3,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_UPDATE_HELPER.l
                move.w  #MARKER_FLIGHT_DECISION,UPDATE_STAGE_MARKER.l
                tst.b   FLIGHT_UPDATE_FLAG.l
                beq.s   .evaluate_flight_branch
                jsr     FLIGHT_BRANCH_HELPER.l
                bra.s   .next_parent_stage
.evaluate_flight_branch:
                jsr     FLIGHT_DECISION_HELPER.l
                tst.l   d0
                beq.s   .flight_branch_false
                move.w  #MARKER_FLIGHT_DECISION_TRUE,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_BRANCH_HELPER.l
                move.w  #MARKER_FLIGHT_FOLLOWUP_TRUE,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_FOLLOWUP_HELPER.l
                bra.s   .next_parent_stage
.flight_branch_false:
                move.w  #MARKER_FLIGHT_DECISION_FALSE,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_FOLLOWUP_HELPER.l
                move.w  #MARKER_FLIGHT_FINAL,UPDATE_STAGE_MARKER.l
                jsr     FLIGHT_BRANCH_HELPER.l
.next_parent_stage:
