; Byte-exact record-delta control-field update $C1B340-$C1B3E9.

                org     $C1B340

RECORD_CONTROL_FLAGS_OFFSET     equ $7C
RECORD_CONTROL_NIBBLE_MASK      equ $0F
RECORD_COMPARE_NIBBLE_OFFSET    equ $39
RECORD_COMPARE_VALUE_OFFSET     equ $2B
RECORD_BIT3_OFFSET              equ $03
RECORD_BIT5_OFFSET              equ $02
RECORD_BIT3                     equ 3
RECORD_BIT5                     equ 5
RECORD_COMPARE_LIMIT            equ $78
CONTROL_COUNTDOWN               equ $C457AE
CONTROL_DELTA_SOURCE            equ $C45870
CONTROL_MODE                    equ $C4584B
CONTROL_ACTIVITY_WORD           equ $C459B6
CONTROL_NEGATIVE_THRESHOLD      equ $F8
CONTROL_POSITIVE_THRESHOLD      equ 8
CONTROL_COMPARE_MODE            equ 2
CONTROL_MODE_SKIP               equ 3

WRITE_CONTROL_VALUE_ONE         equ $C1B4D0
WRITE_CONTROL_VALUE_TWO         equ $C1B4D4
CLEAR_AND_WRITE_CONTROL_ZERO    equ $C1B4D8
WRITE_CONTROL_VALUE_ZERO        equ $C1B4DE
UPDATE_RECORD_CONTROL_STAGE     equ $C25A6A
GATE_THREE_AXIS_CONTROL_UPDATE  equ $C1B3EA
RETURN_FROM_CONTROL_UPDATE      equ $C1B40E

update_control_field_from_record_delta:
                move.b  RECORD_CONTROL_FLAGS_OFFSET(a1),d0
                andi.b  #RECORD_CONTROL_NIBBLE_MASK,d0
                bne.w   RETURN_FROM_CONTROL_UPDATE
                tst.b   CONTROL_COUNTDOWN.l
                bne.b   .finish_control_update
                move.b  CONTROL_DELTA_SOURCE.l,d0
                beq.b   .finish_control_update
                bgt.b   .load_mode
                clr.b   d0
.load_mode:
                move.b  CONTROL_MODE.l,d1
                cmpi.b  #CONTROL_MODE_SKIP,d1
                beq.b   .finish_control_update
                subq.b  #1,d1
                beq.b   .finish_control_update
                move.b  RECORD_COMPARE_NIBBLE_OFFSET(a1),d1
                andi.b  #RECORD_CONTROL_NIBBLE_MASK,d1
                sub.b   RECORD_COMPARE_VALUE_OFFSET(a1),d0
                beq.b   .equal_delta
                bgt.b   .positive_delta
.negative_delta:
                cmpi.b  #CONTROL_NEGATIVE_THRESHOLD,d0
                blt.b   .write_value_two
                cmpi.b  #CONTROL_COMPARE_MODE,d1
                blt.b   .write_value_two
                bsr.w   WRITE_CONTROL_VALUE_ZERO
                bra.b   .finish_control_update
.write_value_two:
                bsr.w   WRITE_CONTROL_VALUE_TWO
                bra.b   .finish_control_update
.positive_delta:
                cmpi.b  #CONTROL_POSITIVE_THRESHOLD,d0
                bgt.b   .write_value_one
                cmpi.b  #CONTROL_COMPARE_MODE,d1
                blt.b   .write_value_one
                bsr.w   WRITE_CONTROL_VALUE_ZERO
                bra.b   .finish_control_update
.write_value_one:
                bsr.w   WRITE_CONTROL_VALUE_ONE
                bra.b   .finish_control_update
.equal_delta:
                cmpi.b  #RECORD_COMPARE_LIMIT,RECORD_COMPARE_VALUE_OFFSET(a1)
                blt.b   .clear_and_write_zero
                btst.b  #RECORD_BIT3,RECORD_BIT3_OFFSET(a1)
                bne.b   .clear_and_write_zero
                btst.b  #RECORD_BIT5,RECORD_BIT5_OFFSET(a1)
                bne.b   .finish_control_update
.clear_and_write_zero:
                bsr.w   CLEAR_AND_WRITE_CONTROL_ZERO
.finish_control_update:
                tst.w   CONTROL_ACTIVITY_WORD.l
                bne.b   GATE_THREE_AXIS_CONTROL_UPDATE
                move.b  CONTROL_MODE.l,d4
                beq.b   .update_record_stage
                subq.b  #1,d4
                bne.b   GATE_THREE_AXIS_CONTROL_UPDATE
.update_record_stage:
                move.l  a1,-(a7)
                jsr     UPDATE_RECORD_CONTROL_STAGE.l
                movea.l (a7)+,a1
