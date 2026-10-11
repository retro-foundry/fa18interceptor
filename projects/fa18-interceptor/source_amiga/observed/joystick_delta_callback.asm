; Byte-exact reconstruction of $C1718E-$C17455 (Hunk 0 +$92DE).
; Callback input: Amiga JOY0DAT hardware counter bytes.
; Output: updates two bounded control accumulators and returns D0=0.

                org     $C1718E

JOY0DAT                         equ $DFF00A
PREVIOUS_JOY0_LOW               equ $C1AC06
PREVIOUS_JOY0_HIGH              equ $C1AC08
CONTROL_ACCUMULATOR_X           equ $C45776
CONTROL_ACCUMULATOR_Y           equ $C45778
CONTROL_LIMIT_X_LOW             equ $C081AC
CONTROL_LIMIT_X_HIGH            equ $C081B0
CONTROL_LIMIT_Y_LOW             equ $C081AE
CONTROL_LIMIT_Y_HIGH            equ $C081B2
CONTROL_UPDATE_COUNT            equ $C45774
HALVE_NEGATIVE_Y_DELTA_FLAG     equ $C457D8
MODE_INDEX_CURRENT              equ $C458A0
MODE_INDEX_TARGET               equ $C458A1
MODE_INDEX_COUNTDOWN            equ $C458A3
MODE_INDEX_STATE                equ $C458A4
MODE_INDEX_SOURCE               equ $C4566C
MODE_INDEX_BUFFER               equ $C08510
MODE_INDEX_COPY_DESTINATION     equ $C45660
MODE_INDEX_VIEWPORT_STATE       equ $C1822A
MODE_INDEX_CALLBACK_LEFT        equ $C1821C
MODE_INDEX_CALLBACK_RIGHT       equ $C18232
MODE_INDEX_LEFT_TABLE           equ $C182BA
MODE_INDEX_RIGHT_TABLE          equ $C182C2
COPY_LONGWORDS                  equ 16
COPY_WORDS                      equ 16
COUNTER_WRAP                    equ $100
COUNTER_MIN_DELTA               equ -128
COUNTER_MAX_DELTA               equ 127
COPY_MODE_BUFFER                equ $C53EC0
POST_INPUT_UPDATE               equ $C24FE8
; VASM rewrites ASL.L #1,D1 as ADD.L D1,D1. Preserve the original opcode.
ASL_LONG_ONE_D1_OPCODE          equ $E381

joystick_delta_callback:
                link.w  a6,#-22
                movem.l d2,-(a7)
                move.w  JOY0DAT.l,d0
                move.w  d0,-4(a6)
                andi.w  #$00ff,d0
                move.w  -4(a6),d1
                asr.w   #8,d1
                andi.w  #$00ff,d1
                move.w  d0,-6(a6)
                sub.w   PREVIOUS_JOY0_LOW.l,d0
                move.w  d1,-8(a6)
                sub.w   PREVIOUS_JOY0_HIGH.l,d1
                move.w  d0,-10(a6)
                move.w  d1,-12(a6)
                cmpi.w  #COUNTER_MIN_DELTA,d0
                bge.s   .x_delta_ready
                addi.w  #COUNTER_WRAP,-10(a6)
                bra.s   .y_delta_normalize
.x_delta_ready:
                move.w  -10(a6),d0
                cmpi.w  #COUNTER_MAX_DELTA,d0
                ble.s   .y_delta_normalize
                subi.w  #COUNTER_WRAP,-10(a6)

.y_delta_normalize:
                move.w  -12(a6),d0
                cmpi.w  #COUNTER_MIN_DELTA,d0
                bge.s   .y_delta_ready
                addi.w  #COUNTER_WRAP,-12(a6)
                bra.s   .optional_y_delta_scale
.y_delta_ready:
                move.w  -12(a6),d0
                cmpi.w  #COUNTER_MAX_DELTA,d0
                ble.s   .optional_y_delta_scale
                subi.w  #COUNTER_WRAP,-12(a6)

.optional_y_delta_scale:
                move.b  HALVE_NEGATIVE_Y_DELTA_FLAG.l,d0
                tst.b   d0
                bne.s   .update_accumulators
                tst.w   -12(a6)
                bpl.s   .update_accumulators
                move.w  -12(a6),d0
                asr.w   #1,d0
                move.w  d0,-12(a6)

.update_accumulators:
                move.w  CONTROL_ACCUMULATOR_X.l,d0
                add.w   -10(a6),d0
                move.w  d0,CONTROL_ACCUMULATOR_X.l
                move.w  CONTROL_ACCUMULATOR_Y.l,d1
                sub.w   -12(a6),d1
                move.w  d1,CONTROL_ACCUMULATOR_Y.l

                move.w  CONTROL_ACCUMULATOR_X.l,d0
                move.w  CONTROL_LIMIT_X_HIGH.l,d1
                cmp.w   d0,d1
                blt.s   .x_high_selected
                move.w  CONTROL_ACCUMULATOR_X.l,d1
.x_high_selected:
                move.w  CONTROL_LIMIT_X_LOW.l,d0
                cmp.w   d1,d0
                bgt.s   .store_x
                move.w  CONTROL_ACCUMULATOR_X.l,d0
                move.w  CONTROL_LIMIT_X_HIGH.l,d1
                cmp.w   d0,d1
                blt.s   .x_low_selected
                move.w  CONTROL_ACCUMULATOR_X.l,d1
.x_low_selected:
                move.l  d1,d0
.store_x:
                move.w  d0,CONTROL_ACCUMULATOR_X.l

                move.w  CONTROL_ACCUMULATOR_Y.l,d0
                move.w  CONTROL_LIMIT_Y_HIGH.l,d1
                cmp.w   d0,d1
                blt.s   .y_high_selected
                move.w  CONTROL_ACCUMULATOR_Y.l,d1
.y_high_selected:
                move.w  CONTROL_LIMIT_Y_LOW.l,d0
                cmp.w   d1,d0
                bgt.s   .store_y
                move.w  CONTROL_ACCUMULATOR_Y.l,d0
                move.w  CONTROL_LIMIT_Y_HIGH.l,d1
                cmp.w   d0,d1
                blt.s   .y_low_selected
                move.w  CONTROL_ACCUMULATOR_Y.l,d1
.y_low_selected:
                move.l  d1,d0
.store_y:
                move.w  d0,CONTROL_ACCUMULATOR_Y.l
                move.w  -6(a6),PREVIOUS_JOY0_LOW.l
                move.w  -8(a6),PREVIOUS_JOY0_HIGH.l
                move.w  CONTROL_UPDATE_COUNT.l,d0
                addq.w  #1,d0
                move.w  d0,CONTROL_UPDATE_COUNT.l

                move.b  MODE_INDEX_CURRENT.l,d0
                move.b  MODE_INDEX_TARGET.l,d1
                cmp.b   d1,d0
                beq.w   .copy_or_update_mode
                move.b  MODE_INDEX_COUNTDOWN.l,d2
                subq.b  #1,d2
                move.b  d2,MODE_INDEX_COUNTDOWN.l
                tst.b   d2
                bpl.w   .post_input_update
                cmp.b   d1,d0
                ble.s   .increment_mode_index
                subq.b  #1,d0
                move.b  d0,MODE_INDEX_CURRENT.l
                move.b  #1,MODE_INDEX_COUNTDOWN.l
                bra.s   .build_mode_buffer
.increment_mode_index:
                move.b  MODE_INDEX_CURRENT.l,d0
                addq.b  #1,d0
                move.b  d0,MODE_INDEX_CURRENT.l
                move.b  #2,MODE_INDEX_COUNTDOWN.l

.build_mode_buffer:
                lea     MODE_INDEX_BUFFER.l,a0
                move.b  MODE_INDEX_CURRENT.l,d0
                ext.w   d0
                ext.l   d0
                moveq   #15,d1
                sub.l   d0,d1
                asl.l   #4,d1
                dc.w    ASL_LONG_ONE_D1_OPCODE ; asl.l #1,d1
                adda.l  d1,a0
                moveq   #COPY_LONGWORDS,d0
                move.l  d0,-(a7)
                move.l  a0,-(a7)
                pea     MODE_INDEX_VIEWPORT_STATE.l
                move.l  a0,-16(a6)
                jsr     COPY_MODE_BUFFER.l
                lea     12(a7),a7
                move.w  MODE_INDEX_SOURCE.l,d0
                moveq   #1,d1
                sub.w   d0,d1
                move.w  d1,-22(a6)
                ext.l   d1
                asl.l   #2,d1
                movea.l d1,a0
                adda.l  #MODE_INDEX_LEFT_TABLE,a0
                move.l  (a0),MODE_INDEX_CALLBACK_LEFT.l
                move.w  -22(a6),d0
                ext.l   d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #MODE_INDEX_RIGHT_TABLE,a0
                move.l  (a0),MODE_INDEX_CALLBACK_RIGHT.l
                moveq   #COPY_LONGWORDS,d0
                move.l  d0,-(a7)
                move.l  -16(a6),-(a7)
                pea     MODE_INDEX_VIEWPORT_STATE.l
                jsr     COPY_MODE_BUFFER.l
                lea     12(a7),a7
                move.w  -22(a6),d0
                ext.l   d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #MODE_INDEX_LEFT_TABLE,a0
                move.l  (a0),MODE_INDEX_CALLBACK_LEFT.l
                move.w  -22(a6),d0
                ext.l   d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #MODE_INDEX_RIGHT_TABLE,a0
                move.l  (a0),MODE_INDEX_CALLBACK_RIGHT.l
                move.b  MODE_INDEX_CURRENT.l,d0
                move.b  MODE_INDEX_TARGET.l,d1
                cmp.b   d1,d0
                bne.s   .post_input_update
                move.l  MODE_INDEX_COPY_DESTINATION.l,-20(a6)
                clr.w   -2(a6)
.copy_mode_words:
                cmpi.w  #COPY_WORDS,-2(a6)
                bge.s   .mode_copy_complete
                movea.l -16(a6),a0
                movea.l -20(a6),a1
                move.w  (a0),(a1)
                addq.l  #2,-16(a6)
                addq.l  #2,-20(a6)
                addq.w  #1,-2(a6)
                bra.s   .copy_mode_words
.mode_copy_complete:
                move.b  #3,MODE_INDEX_STATE.l
                bra.s   .post_input_update

.copy_or_update_mode:
                tst.b   MODE_INDEX_STATE.l
                beq.s   .post_input_update
                moveq   #COPY_LONGWORDS,d0
                move.l  d0,-(a7)
                move.l  MODE_INDEX_COPY_DESTINATION.l,-(a7)
                pea     MODE_INDEX_VIEWPORT_STATE.l
                jsr     COPY_MODE_BUFFER.l
                lea     12(a7),a7

.post_input_update:
                jsr     POST_INPUT_UPDATE.l
                moveq   #0,d0
                movem.l (a7)+,d2
                unlk    a6
                rts
