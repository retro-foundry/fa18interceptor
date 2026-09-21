; Byte-exact normal-update preparation routine $C12098-$C12241.
; Run003 frame 6000 takes C12098->C1223E after 25 instructions.

                org     $C12098

POSTFLIGHT_FLAG                 equ $C45785
CONTROL_BYTE_91                 equ $C45891
CONTROL_RECORD_INDEX             equ $C458DC
CONTROL_RECORD_BASE              equ $C46184
CONTROL_RECORD_STRIDE_SHIFT      equ 9
CONTROL_RECORD_TYPE_OFFSET       equ $62
CONTROL_FLAGS                    equ $C458C6
INPUT_SNAPSHOT_A                 equ $C458B0
POSTFLIGHT_SOURCE                equ $C45833
POSTFLIGHT_CODE                  equ $C45986
POSTFLIGHT_CODE_SCALED           equ $C45988
POSTFLIGHT_RESULT_CODE           equ $C45984
POSTFLIGHT_LATCH                 equ $C45835
UPDATE_BYTE_A7                   equ $C457A7
UPDATE_BYTE_A6                   equ $C457A6
UPDATE_BYTE_58                   equ $C45858
FLIGHT_UPDATE_FLAG               equ $C457AD
UPDATE_TOGGLE_BYTE               equ $C457AC
ACTIVITY_MODE                    equ $C458AE
UPDATE_LONG_LIMIT                equ $C45C42

RECORD_TYPE_MASK                 equ $F0
RECORD_TYPE_CODE                 equ $30
CONTROL_FLAG_1                   equ 1
CONTROL_FLAG_4                   equ 4
POSTFLIGHT_CODE_DEFAULT          equ $32
POSTFLIGHT_CODE_DEFAULT_SCALED   equ $320
POSTFLIGHT_RESULT_A7             equ $A7
POSTFLIGHT_RESULT_B3             equ $B3
POSTFLIGHT_ACTIVITY_MODE         equ 6
UPDATE_LONG_THRESHOLD            equ $24000

PREPARE_INPUT_HELPER             equ $C1B906
CALL_WITH_THREE                  equ $C17B08
PREPARE_UPDATE_HELPER            equ $C08324
FINISH_PREPARE_HELPER            equ $C1BA86
ACTIVITY_LIMIT_HELPER            equ $C082B8

prepare_normal_update_state:
                link.w  a6,#-2
                move.b  POSTFLIGHT_FLAG.l,d0
                tst.b   d0
                bne.s   .select_record
                move.b  CONTROL_BYTE_91.l,d0
                tst.b   d0
                bne.s   .select_record
                jsr     PREPARE_INPUT_HELPER.l
.select_record:
                move.w  CONTROL_RECORD_INDEX.l,d0
                moveq   #CONTROL_RECORD_STRIDE_SHIFT,d1
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #CONTROL_RECORD_BASE,a0
                move.b  CONTROL_RECORD_TYPE_OFFSET(a0),d0
                andi.b  #RECORD_TYPE_MASK,d0
                move.w  CONTROL_FLAGS.l,d1
                move.b  d0,-1(a6)
                btst    #CONTROL_FLAG_1,d1
                beq.w   .after_postflight_mode
                andi.w  #$FFFD,d1
                move.w  d1,CONTROL_FLAGS.l
                move.b  #$FE,INPUT_SNAPSHOT_A.l
                tst.b   POSTFLIGHT_FLAG.l
                beq.s   .copy_postflight_source
                moveq   #0,d0
                move.b  d0,POSTFLIGHT_FLAG.l
                clr.w   POSTFLIGHT_CODE.l
                move.b  d0,UPDATE_BYTE_A6.l
                move.b  #1,POSTFLIGHT_LATCH.l
                bra.s   .call_with_three
.copy_postflight_source:
                move.b  POSTFLIGHT_SOURCE.l,POSTFLIGHT_FLAG.l
                move.w  #POSTFLIGHT_CODE_DEFAULT,POSTFLIGHT_CODE.l
.call_with_three:
                moveq   #3,d0
                move.l  d0,-(a7)
                jsr     CALL_WITH_THREE.l
                addq.l  #4,a7
                cmpi.b  #RECORD_TYPE_CODE,-1(a6)
                bne.s   .scale_postflight_code
                move.w  #POSTFLIGHT_CODE_DEFAULT,POSTFLIGHT_CODE.l
.scale_postflight_code:
                move.w  POSTFLIGHT_CODE.l,d0
                asl.w   #4,d0
                move.w  d0,POSTFLIGHT_CODE_SCALED.l
                jsr     PREPARE_UPDATE_HELPER.l
                clr.b   UPDATE_BYTE_A7.l
                move.b  #$FF,UPDATE_BYTE_58.l
                jsr     FINISH_PREPARE_HELPER.l
                cmpi.b  #RECORD_TYPE_CODE,-1(a6)
                beq.s   .set_result_from_flight_flag
                tst.b   POSTFLIGHT_FLAG.l
                beq.s   .after_postflight_mode
.set_result_from_flight_flag:
                tst.b   FLIGHT_UPDATE_FLAG.l
                beq.s   .set_result_a7
                move.w  #POSTFLIGHT_RESULT_B3,POSTFLIGHT_RESULT_CODE.l
                bra.s   .after_postflight_mode
.set_result_a7:
                move.w  #POSTFLIGHT_RESULT_A7,POSTFLIGHT_RESULT_CODE.l
.after_postflight_mode:
                cmpi.b  #RECORD_TYPE_CODE,-1(a6)
                bne.s   .check_postflight_flag
                move.w  POSTFLIGHT_CODE.l,d0
                tst.w   d0
                bne.s   .check_postflight_flag
                move.w  #POSTFLIGHT_CODE_DEFAULT,POSTFLIGHT_CODE.l
                move.w  #POSTFLIGHT_CODE_DEFAULT_SCALED,POSTFLIGHT_CODE_SCALED.l
                jsr     PREPARE_UPDATE_HELPER.l
                clr.b   UPDATE_BYTE_A7.l
                move.b  #$FF,UPDATE_BYTE_58.l
                jsr     FINISH_PREPARE_HELPER.l
                move.w  #POSTFLIGHT_RESULT_A7,POSTFLIGHT_RESULT_CODE.l
.check_postflight_flag:
                tst.b   POSTFLIGHT_FLAG.l
                beq.s   .return
                move.w  CONTROL_FLAGS.l,d0
                btst    #CONTROL_FLAG_4,d0
                beq.s   .check_activity_mode
                andi.w  #$FFEF,d0
                move.w  d0,CONTROL_FLAGS.l
                move.b  UPDATE_TOGGLE_BYTE.l,d0
                eori.b  #1,d0
                move.b  d0,UPDATE_TOGGLE_BYTE.l
.check_activity_mode:
                move.b  ACTIVITY_MODE.l,d0
                subq.b  #POSTFLIGHT_ACTIVITY_MODE,d0
                bne.s   .return
                cmpi.l  #UPDATE_LONG_THRESHOLD,UPDATE_LONG_LIMIT.l
                bge.s   .return
                move.w  POSTFLIGHT_RESULT_CODE.l,d0
                cmpi.w  #POSTFLIGHT_RESULT_A7,d0
                beq.s   .return
                move.w  #POSTFLIGHT_RESULT_A7,POSTFLIGHT_RESULT_CODE.l
                jsr     ACTIVITY_LIMIT_HELPER.l
.return:
                unlk    a6
                rts
