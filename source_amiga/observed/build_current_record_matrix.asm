; Byte-exact indexed-record matrix setup $C2DAF2-$C2DB17.

                org     $C2DAF2

CONTROL_RECORD_BASE             equ $C46184
CONTROL_RECORD_BYTE_OFFSET      equ $C458DE
MATRIX_OUTPUT                   equ $C45C0E
RECORD_ANGLE_WORD               equ $68
MATRIX_REFERENCE_ANGLE          equ $7080
BUILD_SINGLE_ANGLE_MATRIX       equ $C2E370

build_current_record_matrix:
                lea.l   CONTROL_RECORD_BASE.l,a0
                adda.w  CONTROL_RECORD_BYTE_OFFSET.l,a0
                move.w  #MATRIX_REFERENCE_ANGLE,d1
                move.w  RECORD_ANGLE_WORD(a0),d4
                beq.b   build_selected_record_matrix
                sub.w   d4,d1
                move.w  d1,d4

build_selected_record_matrix:
                lea.l   MATRIX_OUTPUT.l,a1
                bsr.w   BUILD_SINGLE_ANGLE_MATRIX
                rts
