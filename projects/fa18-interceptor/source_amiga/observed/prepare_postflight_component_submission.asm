; Byte-exact C34408-C3444B state, optional offset, and early-exit prefix.
                org     $C34408
POSTFLIGHT_COMPONENT_OUTPUT_A    equ     $C45942
POSTFLIGHT_COMPONENT_OUTPUT_B    equ     $C45944
POSTFLIGHT_OBJECT_STATE          equ     $C461DA
POSTFLIGHT_OBJECT_FLAGS          equ     $C4619A
POSTFLIGHT_OFFSET_TABLE          equ     $C34540
RENDERER_X_OFFSET                equ     $C45988
RENDERER_Y_OFFSET                equ     $C458D8
POSTFLIGHT_SUBMISSION_RETURN     equ     $C3453E

prepare_postflight_component_submission:
                move.w  d4,POSTFLIGHT_COMPONENT_OUTPUT_A.l
                move.w  d2,POSTFLIGHT_COMPONENT_OUTPUT_B.l
                tst.w   POSTFLIGHT_OBJECT_STATE.l
                beq.b   postflight_component_apply_offsets
                move.w  POSTFLIGHT_OBJECT_FLAGS.l,d2
                andi.w  #$1C,d2
                lea     POSTFLIGHT_OFFSET_TABLE.l,a0
                add.w   (a0,d2.w),d0
                add.w   2(a0,d2.w),d1
postflight_component_apply_offsets:
                add.w   RENDERER_X_OFFSET.l,d0
                add.w   RENDERER_Y_OFFSET.l,d1
                move.w  d0,d2
                ble.w   POSTFLIGHT_SUBMISSION_RETURN
                move.w  d1,d3
                ble.w   POSTFLIGHT_SUBMISSION_RETURN
