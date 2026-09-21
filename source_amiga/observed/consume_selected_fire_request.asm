; Byte-exact $C2374C-$C23A25 selected-fire record initializer.
; The bounded run004 $30 Space route enters at the first instruction with
; A1/A2 already identifying destination/source records.

                org     $C2374C

SELECTED_FIRE_REQUEST           equ $C457BA
RECORD_INITIALIZED_FLAG         equ $C457B7
ACTIVE_RECORD_MATCH_FLAG        equ $C45797
RECORD_INITIALIZATION_FLAG      equ $C457C5
SELECTED_FIRE_CONTROL_FLAGS     equ $C45B50
SELECTED_FIRE_AUXILIARY_FLAGS   equ $C45B54
SELECTED_FIRE_LEVEL             equ $C458A7
SELECTED_FIRE_LIMIT_FLAGS       equ $C458B5
SELECTED_FIRE_LIMIT_MASK        equ $C458DA
SELECTED_RECORD_OFFSET          equ $C458DE
SELECTED_FIRE_STATUS            equ $C458B0
INITIALIZATION_COUNTERS         equ $C1AB74
PRIMARY_RECORD_BASE             equ $C46184

RECORD_SELECTION_TYPE_OFFSET    equ $005F
RECORD_MODE_OFFSET              equ $0063
RECORD_INITIALIZATION_KIND      equ $0062
RECORD_COUNTER_A_OFFSET         equ $003E
RECORD_COUNTER_B_OFFSET         equ $0042
RECORD_POSITION_X_OFFSET        equ $0014
RECORD_POSITION_Y_OFFSET        equ $0018
RECORD_POSITION_Z_OFFSET        equ $001C
RECORD_FLAGS_OFFSET             equ $0000
RECORD_SECONDARY_FLAGS_OFFSET   equ $0002
RECORD_TIMER_OFFSET             equ $0026
RECORD_LIFETIME_OFFSET          equ $002C
RECORD_SPEED_OFFSET             equ $004C
RECORD_MATRIX_OFFSET            equ $0056
RECORD_SOURCE_BYTE_OFFSET       equ $0038
RECORD_ORIENTATION_OFFSET       equ $0092
RECORD_ORIENTATION_Y_OFFSET     equ $0094
RECORD_ORIENTATION_Z_OFFSET     equ $0096
RECORD_ORIENTATION_2_OFFSET     equ $0098
RECORD_ORIENTATION_2Y_OFFSET    equ $009A
RECORD_ORIENTATION_2Z_OFFSET    equ $009C
RECORD_ORIENTATION_3_OFFSET     equ $009E
RECORD_ORIENTATION_3Y_OFFSET    equ $00A0
RECORD_ORIENTATION_3Z_OFFSET    equ $00A2

RECORD_TEMPLATE_TABLE           equ $C22048
RECORD_PARAMETER_TABLE          equ $C22188
MODE_THREE_PARAMETER_TABLE      equ $C23A26
MODE_TEN_PARAMETER_TABLE        equ $C23A32
DEFAULT_PARAMETER_TABLE         equ $C23A56
APPLY_VECTOR_ROTATION           equ $C2574A
RECORD_TABLE_INDEX              equ $C459B4
RECORD_STATE_REQUEST_A          equ $C45843
RECORD_STATE_REQUEST_B          equ $C45844
RECORD_NOTIFICATION_CODE        equ $C45858

SELECTED_FIRE_CONTROL_BIT       equ $00004000
SELECTED_FIRE_AUXILIARY_BIT     equ $00000008
MODE_HIGH_NIBBLE_MASK           equ $F0
MODE_THREE                      equ $30

consume_selected_fire_request:
                clr.b   SELECTED_FIRE_REQUEST.l
                move.b  #1,RECORD_INITIALIZED_FLAG.l
                move.l  SELECTED_FIRE_CONTROL_FLAGS.l,d6
                andi.l  #SELECTED_FIRE_CONTROL_BIT,d6
                beq.s   .initialize_record
                andi.l  #$FFFFBFFF,SELECTED_FIRE_CONTROL_FLAGS.l
                ori.l   #SELECTED_FIRE_AUXILIARY_BIT,SELECTED_FIRE_AUXILIARY_FLAGS.l
                bra.s   .initialize_record

.check_selection_allowed:
                tst.b   $C45789.l
                dc.w    $67BE           ; beq.s $C23744 (external prefix)
                cmpi.b  #3,SELECTED_FIRE_LEVEL.l
                bge.s   .level_three_or_more
                cmpi.b  #2,SELECTED_FIRE_LEVEL.l
                bge.s   .level_two
                moveq   #7,d1
                bra.s   .apply_limit
.level_two:
                moveq   #5,d1
                bra.s   .apply_limit
.level_three_or_more:
                moveq   #3,d1
.apply_limit:
                tst.b   SELECTED_FIRE_LIMIT_FLAGS.l
                beq.s   .test_limit
                lsr.w   #1,d1
.test_limit:
                move.w  SELECTED_FIRE_LIMIT_MASK.l,d0
                and.w   d1,d0
                dc.w    $668C           ; bne.s $C23744 (external prefix)

.initialize_record:
                move.b  RECORD_SELECTION_TYPE_OFFSET(a2),d1
                move.b  RECORD_MODE_OFFSET(a2),d0
                andi.b  #MODE_HIGH_NIBBLE_MASK,d0
                cmpi.b  #MODE_THREE,d0
                beq.s   .mode_three
                andi.b  #MODE_HIGH_NIBBLE_MASK,d1
                bra.s   .selection_ready
.mode_three:
                andi.b  #$0F,d1
.selection_ready:
                beq.w   $C23744
                move.l  a2,d0
                subi.l  #PRIMARY_RECORD_BASE,d0
                cmp.w   SELECTED_RECORD_OFFSET.l,d0
                bne.s   .copy_selected_record
                move.b  #8,ACTIVE_RECORD_MATCH_FLAG.l
                move.b  #$FB,SELECTED_FIRE_STATUS.l

.copy_selected_record:
                lea.l   (a2),a4
                lea.l   (a1),a5
                moveq   #$28,d0
.copy_longwords:
                move.l  (a4)+,(a5)+
                dbra    d0,.copy_longwords
                clr.b   5(a1)
                movea.l INITIALIZATION_COUNTERS.l,a0
                move.b  RECORD_MODE_OFFSET(a1),d0
                andi.b  #MODE_HIGH_NIBBLE_MASK,d0
                cmpi.b  #MODE_THREE,d0
                bne.s   .ordinary_kind
                move.b  #1,RECORD_INITIALIZATION_KIND(a1)
                move.w  #$14,d0
                cmpa.l  #PRIMARY_RECORD_BASE,a2
                bne.s   .initialization_kind_ready
                addq.w  #1,RECORD_COUNTER_A_OFFSET(a0)
                move.b  #1,RECORD_INITIALIZATION_FLAG.l
                bra.s   .initialization_kind_ready
.ordinary_kind:
                move.b  #0,RECORD_INITIALIZATION_KIND(a1)
                move.w  #0,d0
                cmpa.l  #PRIMARY_RECORD_BASE,a2
                bne.s   .initialization_kind_ready
                addq.w  #1,RECORD_COUNTER_B_OFFSET(a0)
                move.b  #1,RECORD_INITIALIZATION_FLAG.l
.initialization_kind_ready:
                lea.l   RECORD_TEMPLATE_TABLE.l,a4
                adda.w  d0,a4
                lea.l   RECORD_PARAMETER_TABLE.l,a5
                move.w  RECORD_TABLE_INDEX.l,d3
                add.w   d3,d3
                add.w   d3,d3
                move.w  d3,d4
                add.w   d3,d3
                add.w   d3,d3
                add.w   d4,d3
                adda.w  d3,a5
                movem.l (a4),d2-d6
                movem.l d2-d6,(a5)
                clr.w   RECORD_MATRIX_OFFSET(a1)
                clr.w   RECORD_MATRIX_OFFSET+2(a1)
                clr.w   RECORD_MATRIX_OFFSET+4(a1)
                clr.b   RECORD_MODE_OFFSET+1(a1)
                bclr.b  #2,3(a1)
                move.b  RECORD_INITIALIZATION_KIND(a1),d1
                andi.b  #MODE_HIGH_NIBBLE_MASK,d1
                cmpi.b  #MODE_THREE,d1
                beq.s   .select_mode_three_parameters

                move.b  RECORD_SELECTION_TYPE_OFFSET(a2),d1
                move.b  d1,d2
                move.b  RECORD_MODE_OFFSET(a2),d0
                andi.b  #MODE_HIGH_NIBBLE_MASK,d0
                cmpi.b  #MODE_THREE,d0
                beq.s   .source_mode_three
                andi.b  #MODE_HIGH_NIBBLE_MASK,d1
                andi.b  #$0F,d2
                subi.b  #$10,d1
                move.b  d1,d0
                lsr.b   #4,d0
                bra.s   .source_selection_ready
.source_mode_three:
                andi.b  #$0F,d1
                andi.b  #MODE_HIGH_NIBBLE_MASK,d2
                subq.b  #1,d1
                move.b  d1,d0
                addq.b  #2,d0
.source_selection_ready:
                or.b    d2,d1
                move.b  d1,RECORD_SELECTION_TYPE_OFFSET(a2)
                move.b  #3,RECORD_STATE_REQUEST_A.l
                move.b  #3,RECORD_STATE_REQUEST_B.l
                cmpi.b  #$10,RECORD_INITIALIZATION_KIND(a2)
                beq.s   .use_mode_ten_parameters
                lea.l   DEFAULT_PARAMETER_TABLE.l,a4
                bra.s   .parameter_table_ready
.use_mode_ten_parameters:
                lea.l   MODE_TEN_PARAMETER_TABLE.l,a4
                bra.s   .parameter_table_ready

.select_mode_three_parameters:
                lea.l   MODE_THREE_PARAMETER_TABLE.l,a4
                moveq   #0,d0
                cmpi.b  #MODE_THREE,RECORD_INITIALIZATION_KIND(a1)
                beq.s   .parameter_table_ready
                moveq   #1,d0
.parameter_table_ready:
                ext.w   d0
                add.w   d0,d0
                move.w  d0,d1
                add.w   d1,d1
                add.w   d1,d0
                movem.w (a4,d0.w),d3-d5
                move.w  d3,d6
                move.w  d4,d0
                move.w  d5,d7
                muls.w  RECORD_ORIENTATION_OFFSET(a2),d6
                muls.w  RECORD_ORIENTATION_Y_OFFSET(a2),d0
                muls.w  RECORD_ORIENTATION_Z_OFFSET(a2),d7
                add.l   d6,d0
                add.l   d7,d0
                move.w  d3,d6
                move.w  d4,d1
                move.w  d5,d7
                muls.w  RECORD_ORIENTATION_2_OFFSET(a2),d6
                muls.w  RECORD_ORIENTATION_2Y_OFFSET(a2),d1
                muls.w  RECORD_ORIENTATION_2Z_OFFSET(a2),d7
                add.l   d6,d1
                add.l   d7,d1
                move.w  d4,d2
                muls.w  RECORD_ORIENTATION_3_OFFSET(a2),d3
                muls.w  RECORD_ORIENTATION_3Y_OFFSET(a2),d2
                muls.w  RECORD_ORIENTATION_3Z_OFFSET(a2),d5
                add.l   d3,d2
                add.l   d5,d2
                asr.l   #6,d0
                asr.l   #6,d1
                asr.l   #6,d2
                add.l   d0,RECORD_POSITION_X_OFFSET(a1)
                add.l   d1,RECORD_POSITION_Y_OFFSET(a1)
                add.l   d2,RECORD_POSITION_Z_OFFSET(a1)
                move.w  #$96,d0
                dc.w    $0C29,$0000,$0062 ; cmpi.b #0,$62(a1)
                beq.s   .speed_ready
                move.w  #$C8,d0
.speed_ready:
                move.w  d0,RECORD_SPEED_OFFSET(a1)
                move.w  #$14,RECORD_TIMER_OFFSET(a1)
                dc.w    $0069,$10C0,$0000 ; ori.w #$10C0,0(a1)
                dc.w    $0069,$0100,$0000 ; ori.w #$0100,0(a1)
                dc.w    $0069,$0002,$0000 ; ori.w #$0002,0(a1)
                clr.w   RECORD_MATRIX_OFFSET+$20(a1)
                move.w  RECORD_SECONDARY_FLAGS_OFFSET(a1),d0
                andi.w  #$0080,d0
                beq.s   .secondary_flag_ready
                clr.l   RECORD_COUNTER_B_OFFSET(a1)
.secondary_flag_ready:
                andi.w  #$FF7F,RECORD_SECONDARY_FLAGS_OFFSET(a1)
                move.w  #$FFFF,RECORD_LIFETIME_OFFSET(a1)
                move.b  #$8C,RECORD_NOTIFICATION_CODE.l
                move.b  RECORD_SOURCE_BYTE_OFFSET(a2),RECORD_SOURCE_BYTE_OFFSET(a1)
                move.b  RECORD_INITIALIZATION_KIND(a1),d0
                andi.b  #MODE_HIGH_NIBBLE_MASK,d0
                cmpi.b  #MODE_THREE,d0
                bne.s   .return
                move.w  RECORD_ORIENTATION_Y_OFFSET(a2),d5
                move.w  RECORD_ORIENTATION_2Y_OFFSET(a2),d6
                move.w  RECORD_ORIENTATION_3Y_OFFSET(a2),d7
                asr.w   #2,d5
                asr.w   #2,d6
                asr.w   #2,d7
                cmpi.b  #MODE_THREE,RECORD_INITIALIZATION_KIND(a1)
                beq.s   .rotation_direction_ready
                neg.w   d5
                neg.w   d6
                neg.w   d7
.rotation_direction_ready:
                move.w  #$3C0,d0
                jsr     APPLY_VECTOR_ROTATION.l
                movem.l RECORD_COUNTER_A_OFFSET(a1),d0-d2
                add.l   d0,d5
                add.l   d1,d6
                add.l   d2,d7
                movem.l d5-d7,RECORD_COUNTER_A_OFFSET(a1)
                move.w  #$FFEC,RECORD_SPEED_OFFSET(a1)
.return:
                rts
