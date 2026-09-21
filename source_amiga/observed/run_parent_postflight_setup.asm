; Byte-exact parent post-flight setup slice $C0F124-$C0F1E1.

                org     $C0F124

UPDATE_STAGE_MARKER             equ $C45AD4
MARKER_POSTFLIGHT_PREPARE       equ $C0
MARKER_POSTFLIGHT_DISPATCH      equ $D0
POSTFLIGHT_FLAG                 equ $C45785
POSTFLIGHT_MODE_FLAG            equ $C45836
POSTFLIGHT_RECORD_INDEX         equ $C458DC
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_STRIDE_SHIFT  equ 9
POSTFLIGHT_RECORD_BYTE_62       equ $62
POSTFLIGHT_RECORD_CODE          equ $30
POSTFLIGHT_ACTIVITY_BYTE        equ $C45837

POSTFLIGHT_PREPARE              equ $C11BFC
POSTFLIGHT_SECONDARY_PREPARE    equ $C12950
POSTFLIGHT_SKIP_TARGET          equ $C0F2A8
POSTFLIGHT_CODE_SKIP_TARGET     equ $C0F2DC
POSTFLIGHT_HELPER_1             equ $C30764
POSTFLIGHT_HELPER_2             equ $C309B6
POSTFLIGHT_HELPER_3             equ $C31226
POSTFLIGHT_HELPER_4             equ $C332BC
POSTFLIGHT_HELPER_5             equ $C3112A
POSTFLIGHT_HELPER_6             equ $C30F78
POSTFLIGHT_HELPER_7             equ $C31EB6
POSTFLIGHT_HELPER_8             equ $C31F4C
POSTFLIGHT_HELPER_9             equ $C32178

run_parent_postflight_setup:
                move.w  #MARKER_POSTFLIGHT_PREPARE,UPDATE_STAGE_MARKER.l
                jsr     POSTFLIGHT_PREPARE.l
                jsr     POSTFLIGHT_SECONDARY_PREPARE.l
                move.w  #MARKER_POSTFLIGHT_DISPATCH,UPDATE_STAGE_MARKER.l
                move.b  POSTFLIGHT_FLAG.l,d0
                tst.b   d0
                beq.s   .select_record
                tst.b   POSTFLIGHT_MODE_FLAG.l
                beq.w   POSTFLIGHT_SKIP_TARGET
.select_record:
                move.w  POSTFLIGHT_RECORD_INDEX.l,d0
                moveq   #POSTFLIGHT_RECORD_STRIDE_SHIFT,d1
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #POSTFLIGHT_RECORD_BASE,a0
                move.b  POSTFLIGHT_RECORD_BYTE_62(a0),d0
                cmpi.b  #POSTFLIGHT_RECORD_CODE,d0
                bne.s   .run_postflight_helpers
                tst.b   POSTFLIGHT_MODE_FLAG.l
                beq.w   POSTFLIGHT_CODE_SKIP_TARGET
.run_postflight_helpers:
                jsr     POSTFLIGHT_HELPER_1.l
                jsr     POSTFLIGHT_HELPER_2.l
                jsr     POSTFLIGHT_HELPER_3.l
                jsr     POSTFLIGHT_HELPER_4.l
                jsr     POSTFLIGHT_HELPER_5.l
                jsr     POSTFLIGHT_HELPER_6.l
                move.b  POSTFLIGHT_ACTIVITY_BYTE.l,d0
                tst.b   d0
                bgt.s   .run_activity_helpers_1
                move.w  -2(a6),d0
                andi.w  #3,d0
                cmpi.w  #1,d0
                ble.s   .after_activity_helpers_1
.run_activity_helpers_1:
                jsr     POSTFLIGHT_HELPER_7.l
                jsr     POSTFLIGHT_HELPER_8.l
.after_activity_helpers_1:
                move.b  POSTFLIGHT_ACTIVITY_BYTE.l,d0
                tst.b   d0
                bgt.s   .run_activity_helper_2
                move.w  -2(a6),d0
                andi.w  #3,d0
                cmpi.w  #2,d0
                bge.s   .after_activity_helper_2
.run_activity_helper_2:
                jsr     POSTFLIGHT_HELPER_9.l
.after_activity_helper_2:
