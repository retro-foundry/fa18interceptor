; Byte-exact observed continuation in the $C13D84 control-record update,
; $C14022-$C140A9.  Field roles beyond this data flow remain unknown.

                org     $C14022

NEGATIVE_DELTA_ENABLE          equ     $C457D8
CONTROL_ACCUMULATOR            equ     $C45778
CONTROL_ACCUMULATOR_COMPANION  equ     $C4577C
CONTROL_STAGE_MODE             equ     $C458A6

update_c13e10_control_stage_thresholds:
                tst.b   NEGATIVE_DELTA_ENABLE.l
                beq.b   $C1406C
                move.w  CONTROL_ACCUMULATOR.l,d0
                cmpi.w  #$3C0,d0
                bge.b   $C1403E
                cmpi.w  #$10,-$28(a6)
                bge.b   $C1406C
                move.w  -$24(a6),d0
                asl.w   #3,d0
                move.w  d0,CONTROL_ACCUMULATOR_COMPANION.l
                move.w  CONTROL_ACCUMULATOR.l,d1
                tst.w   d1
                bmi.b   $C1406C
                move.w  d0,CONTROL_ACCUMULATOR.l
                bra.b   $C1406C
                movea.l -$c(a6),a0
                move.b  (a0),d0
                andi.b  #$F0,d0
                move.b  d0,(a0)
                addq.b  #1,d0
                move.b  d0,(a0)
control_stage_continue:
                tst.w   -$32(a6)
                bne.w   $C1414E
                move.b  CONTROL_STAGE_MODE.l,d0
                subq.b  #2,d0
                beq.w   $C1414E
                tst.b   NEGATIVE_DELTA_ENABLE.l
                beq.w   $C1414E
                move.w  CONTROL_ACCUMULATOR.l,d0
                cmpi.w  #$3C0,d0
                bge.b   $C140A0
                cmpi.w  #$10,-$28(a6)
                ble.w   $C1414E
control_stage_nonnegative_accumulator:
                move.w  CONTROL_ACCUMULATOR.l,d0
                tst.w   d0
                bpl.b   $C140BA
