; Byte-exact observed-entry slice $C1D91A-$C1D9D7 (Hunk 8 +$1662).
; The branch to $C1D90A is outside this slice and was not taken in run001.

                org     $C1D91A

INPUT_OFFSET_WORD               equ $C45A72
INPUT_OFFSET_LONG               equ $C45A78
INPUT_OFFSET_SECOND_WORD        equ $C45A76
VARIABLE_FIXED_SHIFT            equ $C45AB8
OPTIONAL_SIGN_MODE_FLAG         equ $C458BB
ALTERNATE_LONG_MAGNITUDE        equ $C45B3C
FIXED_SCALE_TABLE               equ $C1D9D8
FIXED_RESULT_WORD               equ $C45B40
MAX_INTERMEDIATE_MAGNITUDE      equ $7FFF0
MAX_RESULT_WORD                 equ $7FFF
INTERMEDIATE_SHIFT              equ 4
TABLE_INDEX_SHIFT               equ 6
FIXED_SCALE_TABLE_SHIFT         equ 14
PREVIOUS_FIXED_STAGE_ENTRY      equ $C1D90A

fixed_point_stage_tail:
                movem.l d5/d7,-(a7)
                move.w  INPUT_OFFSET_WORD.l,d1
                move.l  INPUT_OFFSET_LONG.l,d6
                move.w  INPUT_OFFSET_SECOND_WORD.l,d7
                move.w  VARIABLE_FIXED_SHIFT.l,d5
                asr.w   d5,d1
                asr.l   d5,d6
                asr.w   d5,d7
                add.w   d1,d2
                bpl.s   .first_magnitude_ready
                neg.w   d2
.first_magnitude_ready:
                tst.b   OPTIONAL_SIGN_MODE_FLAG.l
                beq.s   .combined_long_magnitude
                move.l  ALTERNATE_LONG_MAGNITUDE.l,d3
                asr.l   #8,d3
                bra.s   .long_magnitude_ready
.combined_long_magnitude:
                ext.l   d3
                add.l   d6,d3
.long_magnitude_ready:
                bpl.s   .long_magnitude_nonnegative
                neg.l   d3
.long_magnitude_nonnegative:
                cmpi.l  #MAX_INTERMEDIATE_MAGNITUDE,d3
                bge.s   PREVIOUS_FIXED_STAGE_ENTRY
                add.w   d7,d4
                bpl.s   .second_magnitude_ready
                neg.w   d4
.second_magnitude_ready:
                asr.w   #INTERMEDIATE_SHIFT,d2
                asr.l   #INTERMEDIATE_SHIFT,d3
                asr.w   #INTERMEDIATE_SHIFT,d4
                movem.l (a7)+,d5/d7
                movem.l d5/a0,-(a7)
                moveq   #FIXED_SCALE_TABLE_SHIFT,d5
                lea     FIXED_SCALE_TABLE.l,a0
                cmp.w   d2,d3
                ble.s   .select_table_numerator
                exg.l   d2,d3
.select_table_numerator:
                ext.l   d3
                beq.s   .table_index_ready
                asl.l   #8,d3
                tst.w   d2
                bne.s   .divide_table_index
                moveq   #0,d3
                bra.s   .table_index_ready
.divide_table_index:
                divu.w  d2,d3
                add.w   d3,d3
.table_index_ready:
                move.w  (a0,d3.w),d3
                mulu.w  d3,d2
                ext.l   d4
                asl.l   d5,d4
                cmp.l   d2,d4
                ble.s   .scale_values
                exg.l   d2,d4
.scale_values:
                asr.l   d5,d2
                tst.w   d2
                bne.s   .divide_scaled_values
                moveq   #0,d4
                bra.s   .scale_index_ready
.divide_scaled_values:
                divu.w  d2,d4
                asr.w   #TABLE_INDEX_SHIFT,d4
                add.w   d4,d4
.scale_index_ready:
                move.w  (a0,d4.w),d1
                mulu.w  d2,d1
                asr.l   d5,d1
                movem.l (a7)+,d5/a0
                cmpi.l  #MAX_RESULT_WORD,d1
                ble.s   .result_in_range
                move.w  #MAX_RESULT_WORD,d1
.result_in_range:
.store_result:
                move.w  d1,FIXED_RESULT_WORD.l
                rts
