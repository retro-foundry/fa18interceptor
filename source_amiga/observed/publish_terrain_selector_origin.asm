; Byte-exact active terrain-selector-origin producer $C29042-$C291D3.
; The accepted path derives three mutable longwords from the active control
; record and a matrix helper, then stores them at C45C3E/C45C42/C45C46.  These
; are proven terrain-template selector inputs, not proven aircraft/world axes.

                org     $C29042

ORIGIN_ENABLE               equ     $C45785
ORIGIN_GATE_B               equ     $C457B5
ORIGIN_GATE_A               equ     $C457AE
ORIGIN_GATE_MODE            equ     $C457AD
ORIGIN_DETAIL_MODE          equ     $C458AE
ORIGIN_DETAIL_INDEX         equ     $C458B2
ORIGIN_RECORD_OFFSET        equ     $C458DE
ORIGIN_RECORD_BASE          equ     $C46184
ORIGIN_ANGLE_HISTORY        equ     $C4592A
ORIGIN_TEMP_TRIPLE          equ     $C45C56
TERRAIN_SELECTOR_ORIGIN     equ     $C45C3E

publish_terrain_selector_origin:
                jsr     $C2DAF2.l
                tst.b   ORIGIN_ENABLE.l
                beq.b   $C29040
                tst.b   ORIGIN_GATE_B.l
                beq.b   $C29040
                tst.b   ORIGIN_GATE_A.l
                bne.b   $C29040
                tst.b   ORIGIN_GATE_MODE.l
                beq.b   .select_matrix_path
                tst.b   ORIGIN_DETAIL_MODE.l
                bne.b   .select_matrix_path
                lea.l   ORIGIN_RECORD_BASE.l,a0
                adda.w  ORIGIN_RECORD_OFFSET.l,a0
                move.l  $14(a0),d5
                move.l  $C45C42.l,d6
                move.l  $1c(a0),d7
                bra.w   .publish_origin
.select_matrix_path:
                cmpi.b  #5,ORIGIN_DETAIL_MODE.l
                beq.b   .load_active_record
                cmpi.b  #2,ORIGIN_DETAIL_MODE.l
                bge.w   $C291D4
.load_active_record:
                lea.l   ORIGIN_RECORD_BASE.l,a0
                adda.w  ORIGIN_RECORD_OFFSET.l,a0
                move.b  $62(a0),d0
                andi.b  #$f0,d0
                move.b  d0,d7
                cmpi.b  #$30,d0
                bne.b   .select_matrix_table
                lea.l   $C29004(pc),a1
                bra.w   .run_matrix_helper
.select_matrix_table:
                move.w  $68(a0),d0
                move.w  ORIGIN_ANGLE_HISTORY.l,d1
                sub.w   d0,d1
                bge.b   .absolute_angle_delta
                neg.w   d1
.absolute_angle_delta:
                move.w  d0,ORIGIN_ANGLE_HISTORY.l
                cmpi.w  #$1c20,d1
                blt.b   .select_record_type_table
                cmpi.w  #$5460,d1
                bgt.b   .select_record_type_table
                move.b  ORIGIN_DETAIL_INDEX.l,d0
                beq.b   .select_record_type_table
                cmpi.b  #8,d0
                bge.b   .select_record_type_table
                cmpi.b  #4,d0
                beq.b   .select_record_type_table
                addq.b  #4,d0
                cmpi.b  #8,d0
                blt.b   .publish_detail_index
                subq.b  #8,d0
.publish_detail_index:
                move.b  d0,ORIGIN_DETAIL_INDEX.l
.select_record_type_table:
                cmpi.b  #$11,$62(a0)
                beq.b   .table_11
                cmpi.b  #$14,$62(a0)
                beq.b   .table_14
                lea.l   $C28F50(pc),a1
                bra.b   .run_matrix_helper
.table_11:
                lea.l   $C28F8C(pc),a1
                bra.b   .run_matrix_helper
.table_14:
                lea.l   $C28FC8(pc),a1
.run_matrix_helper:
                move.b  ORIGIN_DETAIL_INDEX.l,d0
                ext.w   d0
                add.w   d0,d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d1,d0
                movem.w (a1,d0.w),d3-d5
                move.b  ORIGIN_ENABLE.l,d0
                blt.b   .call_matrix_helper
                ext.w   d0
                cmpi.w  #1,d0
                bgt.b   .shift_matrix_inputs
                move.w  d3,d0
                move.w  d4,d1
                move.w  d5,d2
                asr.w   #1,d0
                asr.w   #1,d1
                asr.w   #1,d2
                add.w   d0,d3
                add.w   d1,d4
                add.w   d2,d5
                bra.b   .call_matrix_helper
.shift_matrix_inputs:
                subq.w  #1,d0
                asl.w   d0,d3
                asl.w   d0,d4
                asl.w   d0,d5
.call_matrix_helper:
                move.l  a0,-(a7)
                move.b  ORIGIN_DETAIL_INDEX.l,d0
                cmpi.b  #$30,d7
                beq.b   .matrix_helper_a
                tst.b   d0
                beq.b   .matrix_helper_b
                cmpi.b  #4,d0
                beq.b   .matrix_helper_b
.matrix_helper_a:
                jsr     $C091A8.l
                bra.b   .matrix_complete
.matrix_helper_b:
                jsr     $C091CE.l
.matrix_complete:
                movem.l d0-d2,ORIGIN_TEMP_TRIPLE.l
                movem.l ORIGIN_TEMP_TRIPLE.l,d5-d7
                movea.l (a7)+,a0
                move.b  $4(a0),d0
                andi.b  #$c0,d0
                bne.b   .have_origin_floor
                moveq   #0,d0
                bra.b   .scale_origin_floor
.have_origin_floor:
                move.w  $4e(a0),d0
.scale_origin_floor:
                dc.w    $0640,$0007       ; addi.w #7,d0; preserve original non-addq form
                ext.l   d0
                asl.l   #8,d0
                cmp.l   d0,d6
                bge.b   .publish_origin
                move.l  d0,d6
.publish_origin:
                movem.l d5-d7,TERRAIN_SELECTOR_ORIGIN.l
                bra.w   $C295B6
