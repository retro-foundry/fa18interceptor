; Byte-exact activity-gated parent stages $C0F1E2-$C0F2A7.

                org     $C0F1E2

ACTIVITY_BYTE                   equ $C45837
STAGE_SIGNED_LONG               equ $C45A66
UPDATE_STAGE_MARKER             equ $C45AD4
MARKER_ACTIVITY_STAGE           equ $1A0
MARKER_ACTIVITY_COMPLETE        equ $1D0
ACTIVITY_LONG_LIMIT              equ $FFFF8000

ACTIVITY_HELPER_1               equ $C3201A
ACTIVITY_HELPER_2               equ $C3212A
ACTIVITY_HELPER_3               equ $C30918
ACTIVITY_HELPER_4               equ $C3003A
ACTIVITY_HELPER_5               equ $C328A8
ACTIVITY_HELPER_6               equ $C30A00
ACTIVITY_HELPER_7               equ $C321D2
ACTIVITY_HELPER_8               equ $C32260
ACTIVITY_HELPER_9               equ $C31ACC
ACTIVITY_HELPER_10              equ $C30D34
ACTIVITY_HELPER_11              equ $C31A64
ACTIVITY_HELPER_12              equ $C322EE
ACTIVITY_HELPER_13              equ $C30B5C
NEXT_PARENT_BRANCH              equ $C0F2DC

run_parent_activity_stages:
                move.b  ACTIVITY_BYTE.l,d0
                tst.b   d0
                bgt.s   .run_helper_1
                move.w  -2(a6),d0
                andi.w  #3,d0
                cmpi.w  #2,d0
                blt.s   .run_helper_1
                cmpi.l  #ACTIVITY_LONG_LIMIT,STAGE_SIGNED_LONG.l
                ble.s   .after_helper_1
.run_helper_1:
                jsr     ACTIVITY_HELPER_1.l
.after_helper_1:
                move.b  ACTIVITY_BYTE.l,d0
                tst.b   d0
                bgt.s   .run_helper_2
                move.w  -2(a6),d0
                andi.w  #7,d0
                cmpi.w  #2,d0
                bge.s   .after_helper_2
.run_helper_2:
                jsr     ACTIVITY_HELPER_2.l
.after_helper_2:
                move.b  ACTIVITY_BYTE.l,d0
                tst.b   d0
                bgt.s   .run_helper_3
                move.w  -2(a6),d0
                andi.w  #$F,d0
                cmpi.w  #2,d0
                bge.s   .after_helper_3
.run_helper_3:
                jsr     ACTIVITY_HELPER_3.l
.after_helper_3:
                move.w  #MARKER_ACTIVITY_STAGE,UPDATE_STAGE_MARKER.l
                jsr     ACTIVITY_HELPER_4.l
                jsr     ACTIVITY_HELPER_5.l
                jsr     ACTIVITY_HELPER_6.l
                jsr     ACTIVITY_HELPER_7.l
                jsr     ACTIVITY_HELPER_8.l
                jsr     ACTIVITY_HELPER_9.l
                jsr     ACTIVITY_HELPER_10.l
                jsr     ACTIVITY_HELPER_11.l
                jsr     ACTIVITY_HELPER_12.l
                jsr     ACTIVITY_HELPER_13.l
                move.b  ACTIVITY_BYTE.l,d0
                tst.b   d0
                bmi.s   .activity_complete
                subq.b  #1,d0
                move.b  d0,ACTIVITY_BYTE.l
.activity_complete:
                move.w  #MARKER_ACTIVITY_COMPLETE,UPDATE_STAGE_MARKER.l
                bra.s   NEXT_PARENT_BRANCH
