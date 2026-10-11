; Byte-exact reconstruction of $C2DB18-$C2DCC1 (Hunk 32 +$710).
; Observed disabled branch of dispatch_matrix_update_route.

                org     $C2DB18

CONTROL_RECORD_BASE          equ $C46184
ACTIVE_RECORD_OFFSET          equ $C458DE
MATRIX_ROUTE_STATE            equ $C45785
MATRIX_SELECTOR               equ $C457A7
MATRIX_MODE                   equ $C457B1
RECORD_ANGLE_TRIPLE_OFFSET    equ $66
RECORD_SINGLE_ANGLE_OFFSET    equ $68
RECORD_TYPE_OFFSET            equ $62
RECORD_TYPE_MASK              equ $F0
SPECIAL_RECORD_TYPE           equ $30
MATRIX_AUXILIARY_CACHE        equ $C45A88
FIRST_MATRIX_CACHE            equ $C45BEA
SECOND_MATRIX_CACHE           equ $C45BD8
THIRD_MATRIX_CACHE            equ $C45BFC
TEMP_MATRIX_CACHE             equ $C45C20
RECORD_TABLE_NORMAL           equ $C2DCC2
RECORD_TABLE_SPECIAL          equ $C2DD04

SELECTOR_BIAS                 equ $0B
SPECIAL_SELECTOR_VALUE        equ $0C
MODE_ONE                      equ 1
MODE_TWO_BIAS                 equ 2
ANGLE_LOW_BOUND               equ $0230
ANGLE_MID_BOUND               equ $19F0
ANGLE_HIGH_BOUND              equ $1E50
ANGLE_UPPER_BOUND             equ $6E50
ANGLE_VALUE_A                 equ $1C20
ANGLE_VALUE_B                 equ $5460
ANGLE_VALUE_C                 equ $5690
ANGLE_VALUE_D                 equ $3610
ANGLE_VALUE_E                 equ $3840
SECOND_ANGLE_VALUE_A          equ $0280
SECOND_ANGLE_VALUE_B          equ $6E00

build_single_angle_matrix     equ $C2E346
compose_three_angle_matrix    equ $C2E3DE
scale_matrix_rows             equ $C2E5AC
transform_matrix_record       equ $C2DEE0

update_control_record_matrix_route:
                lea.l   CONTROL_RECORD_BASE.l,a0
                adda.w  ACTIVE_RECORD_OFFSET.l,a0
                move.w  RECORD_SINGLE_ANGLE_OFFSET(a0),d4
                lea.l   THIRD_MATRIX_CACHE.l,a1
                bsr.w   build_single_angle_matrix
                lea.l   CONTROL_RECORD_BASE.l,a0
                adda.w  ACTIVE_RECORD_OFFSET.l,a0
                tst.b   MATRIX_ROUTE_STATE.l
                bne.b   .select_matrix_angles
                movem.w RECORD_ANGLE_TRIPLE_OFFSET(a0),d0/d2/d4
                lea.l   FIRST_MATRIX_CACHE.l,a1
                move.l  a0,-(a7)
                bsr.w   compose_three_angle_matrix
                movea.l (a7)+,a0
.select_matrix_angles:
                moveq   #0,d0
                moveq   #0,d2
                moveq   #0,d4
                move.b  RECORD_TYPE_OFFSET(a0),d3
                andi.b  #RECORD_TYPE_MASK,d3
                move.b  MATRIX_SELECTOR.l,d1
                beq.b   .selector_zero
                cmpi.b  #SELECTOR_BIAS,d1
                ble.w   .load_angle_tuple
                subi.b  #SELECTOR_BIAS,d1
                cmpi.b  #SPECIAL_RECORD_TYPE,d3
                bne.b   .select_normal_angle
                bra.w   .select_special_angle
.selector_zero:
                cmpi.b  #SPECIAL_RECORD_TYPE,d3
                bne.b   .select_mode
                moveq   #SPECIAL_SELECTOR_VALUE,d1
                bra.w   .load_angle_tuple
.select_mode:
                move.b  MATRIX_MODE.l,d1
                cmpi.b  #MODE_ONE,d1
                bgt.b   .select_high_mode
                movem.w RECORD_ANGLE_TRIPLE_OFFSET(a0),d0/d2/d4
                movem.l d0/d2/d4,MATRIX_AUXILIARY_CACHE.l
                bra.w   .build_second_matrix
.select_high_mode:
                subq.b  #MODE_TWO_BIAS,d1
                beq.w   .second_angle_value_b
                bra.w   .second_angle_value_a
.select_normal_angle:
                subq.b  #1,d1
                beq.b   .normal_angle_variant_two
                move.w  RECORD_ANGLE_TRIPLE_OFFSET(a0),d0
                cmpi.w  #ANGLE_LOW_BOUND,d0
                blt.b   .normal_angle_value_low
                cmpi.w  #ANGLE_UPPER_BOUND,d0
                blt.b   .normal_angle_value_mid
.normal_angle_value_low:
                move.w  #ANGLE_MID_BOUND,d0
                bra.w   .build_temp_matrix
.normal_angle_value_mid:
                move.w  #ANGLE_VALUE_A,d0
                bra.w   .build_temp_matrix
.normal_angle_variant_two:
                move.w  RECORD_ANGLE_TRIPLE_OFFSET(a0),d0
                cmpi.w  #ANGLE_LOW_BOUND,d0
                blt.b   .normal_angle_variant_two_low
                cmpi.w  #ANGLE_UPPER_BOUND,d0
                blt.b   .normal_angle_variant_two_mid
.normal_angle_variant_two_low:
                move.w  #ANGLE_VALUE_C,d0
                bra.b   .build_temp_matrix
.normal_angle_variant_two_mid:
                move.w  #ANGLE_VALUE_B,d0
                bra.b   .build_temp_matrix
.select_special_angle:
                subq.b  #1,d1
                beq.b   .special_angle_variant_two
                move.w  RECORD_ANGLE_TRIPLE_OFFSET(a0),d0
                cmpi.w  #ANGLE_HIGH_BOUND,d0
                bge.b   .special_angle_zero
                cmpi.w  #ANGLE_MID_BOUND,d0
                ble.b   .special_angle_zero
                move.w  #ANGLE_LOW_BOUND,d0
                bra.b   .build_temp_matrix
.special_angle_zero:
                move.w  #0,d0
                bra.b   .build_temp_matrix
.special_angle_variant_two:
                move.w  RECORD_ANGLE_TRIPLE_OFFSET(a0),d0
                cmpi.w  #ANGLE_HIGH_BOUND,d0
                bge.b   .special_angle_variant_two_default
                cmpi.w  #ANGLE_MID_BOUND,d0
                ble.b   .special_angle_variant_two_default
                move.w  #ANGLE_VALUE_D,d0
                bra.b   .build_temp_matrix
.special_angle_variant_two_default:
                move.w  #ANGLE_VALUE_E,d0
                bra.b   .build_temp_matrix
.second_angle_value_a:
                move.w  #SECOND_ANGLE_VALUE_A,d2
                bra.b   .build_temp_matrix
.second_angle_value_b:
                move.w  #SECOND_ANGLE_VALUE_B,d2
                bra.b   .build_temp_matrix
.load_angle_tuple:
                cmpi.b  #SPECIAL_RECORD_TYPE,d3
                bne.b   .select_normal_tuple_table
                lea.l   RECORD_TABLE_SPECIAL.l,a1
                bra.b   .read_tuple
.select_normal_tuple_table:
                lea.l   RECORD_TABLE_NORMAL.l,a1
.read_tuple:
                subq.w  #1,d1
                ext.w   d1
                add.w   d1,d1
                move.w  d1,d0
                add.w   d1,d1
                add.w   d0,d1
                adda.w  d1,a1
                move.w  (a1)+,d0
                move.w  (a1)+,d2
                move.w  (a1),d4
.build_temp_matrix:
                move.l  a0,-(a7)
                movem.w d0/d2/d4,-(a7)
                lea.l   TEMP_MATRIX_CACHE.l,a1
                bsr.w   compose_three_angle_matrix
                lea.l   TEMP_MATRIX_CACHE.l,a1
                bsr.w   scale_matrix_rows
                movem.w (a7)+,d0/d2/d4
                movea.l (a7)+,a0
                lea.l   $80(a0),a4
                bsr.w   transform_matrix_record
                movem.l d4-d6,MATRIX_AUXILIARY_CACHE.l
.build_second_matrix:
                move.w  $C45A8A,d0
                move.w  $C45A8E,d2
                move.w  $C45A92,d4
                lea.l   SECOND_MATRIX_CACHE.l,a1
                bsr.w   compose_three_angle_matrix
                lea.l   SECOND_MATRIX_CACHE.l,a1
                bsr.w   scale_matrix_rows
                rts
