; Byte-exact $C2641E-$C2651B helper. Record/table ownership is unassigned.

                org     $C2641E

SHARED_RECORD_BASE             equ $C46184
SHARED_RECORD_INDEX            equ $C459B6
CONTROL_STATE_BYTE             equ $C45784
TRIG_WORD_TABLE                equ $C3E5E8
PRIMARY_LIMIT_TABLE            equ $C263DC
SECONDARY_LIMIT_TABLE          equ $C2639C

RECORD_INPUT_WORD_66           equ $66
RECORD_INPUT_WORD_6C           equ $6C
RECORD_INPUT_WORD_6E           equ $6E
RECORD_FLAG_BYTE_03            equ 3
RECORD_FLAG_BYTE_7C            equ $7C
RECORD_OUTPUT_WORD_76          equ $76
RECORD_OUTPUT_WORD_78          equ $78

ABSOLUTE_SHIFT                 equ 7
ANGLE_INDEX_SHIFT              equ 3
TRIG_COMPONENT_SHIFT           equ 6
PRODUCT_SHIFT                  equ 3
OUTPUT_FAST_SHIFT              equ 5
OUTPUT_SLOW_SHIFT              equ 8
TABLE_INDEX_LIMIT              equ $001E
ANGLE_INDEX_LIMIT              equ $0708
ANGLE_INDEX_BIAS               equ $0384
ANGLE_INDEX_WRAP               equ $0E10
TRIG_SCALE_BIAS                equ $0100
OUTPUT_DELTA_LIMIT             equ $00FF

update_indexed_shared_record_fields:
                clr.w   RECORD_OUTPUT_WORD_76(a1)
                clr.w   RECORD_OUTPUT_WORD_78(a1)
                rts

.active:
                lea.l   SHARED_RECORD_BASE.l,a1
                adda.w  SHARED_RECORD_INDEX.l,a1
                tst.b   CONTROL_STATE_BYTE.l
                beq.s   update_indexed_shared_record_fields

                move.w  RECORD_INPUT_WORD_6C(a1),d0
                bge.s   .input_6c_nonnegative
                neg.w   d0
.input_6c_nonnegative:
                asr.w   #ABSOLUTE_SHIFT,d0
                cmpi.w  #TABLE_INDEX_LIMIT,d0
                ble.s   .input_6c_in_range
                moveq   #TABLE_INDEX_LIMIT,d0
.input_6c_in_range:
                lea     PRIMARY_LIMIT_TABLE(pc),a0
                add.w   d0,d0
                move.w  (a0,d0.w),d1

                lea.l   TRIG_WORD_TABLE.l,a0
                move.w  RECORD_INPUT_WORD_66(a1),d2
                asr.w   #ANGLE_INDEX_SHIFT,d2
                cmpi.w  #ANGLE_INDEX_LIMIT,d2
                bgt.s   .wrapped_angle_index

                neg.w   d2
                addi.w  #ANGLE_INDEX_BIAS,d2
                add.w   d2,d2
                move.w  (a0,d2.w),d3
                asr.w   #TRIG_COMPONENT_SHIFT,d3
                neg.w   d3
                addi.w  #TRIG_SCALE_BIAS,d3
                move.w  2(a0,d2.w),d4
                asr.w   #TRIG_COMPONENT_SHIFT,d4
                bra.s   .apply_primary_component

.wrapped_angle_index:
                neg.w   d2
                addi.w  #ANGLE_INDEX_WRAP,d2
                neg.w   d2
                addi.w  #ANGLE_INDEX_BIAS,d2
                add.w   d2,d2
                move.w  (a0,d2.w),d3
                asr.w   #TRIG_COMPONENT_SHIFT,d3
                neg.w   d3
                addi.w  #TRIG_SCALE_BIAS,d3
                neg.w   d3
                move.w  2(a0,d2.w),d4
                asr.w   #TRIG_COMPONENT_SHIFT,d4

.apply_primary_component:
                muls.w  d3,d1
                asr.l   #PRODUCT_SHIFT,d1
                btst.b  #7,RECORD_FLAG_BYTE_03(a1)
                bne.s   .clear_output_78
                tst.b   RECORD_FLAG_BYTE_7C(a1)
                blt.s   .adjust_primary_component
                move.w  d1,d0
                asr.w   #2,d0
                sub.w   d0,d1

.adjust_primary_component:
                move.w  RECORD_OUTPUT_WORD_78(a1),d0
                move.w  d0,d2
                cmp.w   d1,d0
                ble.s   .decrease_output_78
                sub.w   d1,d0
                cmpi.w  #OUTPUT_DELTA_LIMIT,d0
                bgt.s   .large_increase
                tst.w   d2
                beq.s   .store_output_78
                blt.s   .clear_output_78
                subq.w  #1,d2
                bra.s   .store_output_78
.large_increase:
                asr.w   #OUTPUT_SLOW_SHIFT,d0
                bra.s   .subtract_output_delta
.clear_output_78:
                moveq   #0,d2
                bra.s   .store_output_78
.decrease_output_78:
                sub.w   d1,d0
                asr.w   #OUTPUT_FAST_SHIFT,d0
.subtract_output_delta:
                sub.w   d0,d2
.store_output_78:
                move.w  d2,RECORD_OUTPUT_WORD_78(a1)

                move.w  RECORD_INPUT_WORD_6E(a1),d0
                bge.s   .input_6e_nonnegative
                neg.w   d0
.input_6e_nonnegative:
                asr.w   #ABSOLUTE_SHIFT,d0
                cmpi.w  #TABLE_INDEX_LIMIT,d0
                ble.s   .input_6e_in_range
                moveq   #TABLE_INDEX_LIMIT,d0
.input_6e_in_range:
                lea     SECONDARY_LIMIT_TABLE(pc),a0
                add.w   d0,d0
                move.w  (a0,d0.w),d2
                move.w  RECORD_OUTPUT_WORD_76(a1),d0
                move.w  d0,d1
                sub.w   d2,d0
                asr.w   #TRIG_COMPONENT_SHIFT,d0
                sub.w   d0,d1
                move.w  d2,RECORD_OUTPUT_WORD_76(a1)
                rts
