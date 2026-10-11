; Byte-exact observed message-sequence/layout controller $C32BD2-$C32CEC.
; It is a text/layout dataflow block, not a crash or qualification-state claim.

                org     $C32BD2

MESSAGE_SELECTOR_SEQUENCE       equ     $C4574A
MESSAGE_SELECTOR_CURSOR         equ     $C457C6
MESSAGE_LAYOUT_TRIPLET          equ     $C4570A
MESSAGE_LAYOUT_OUTPUT_TRIPLET   equ     $C456FE
MESSAGE_LAYOUT_OFFSET           equ     $C4573E
MESSAGE_SEQUENCE_STATE          equ     $C45744
MESSAGE_GLYPH_OFFSET            equ     $C45746
MESSAGE_GLYPH_SENTINEL          equ     $C45748
MESSAGE_ACTIVE_FLAG             equ     $C457C3
MESSAGE_RECORD_ATTRIBUTE        equ     $C457DC
MESSAGE_RECORD_SUBTYPE          equ     $C457DB
MESSAGE_GLYPH_REPEAT_COUNT      equ     $C457D7
MESSAGE_SEQUENCE_MODE           equ     $C457E0
MESSAGE_DELAY_OR_COUNT          equ     $C457DF
MESSAGE_LAYOUT_INDEX            equ     $C45952
MESSAGE_GLYPH_HELPER            equ     $C3316A
CONTINUE_GLYPH_COMPOSITOR       equ     $C330F4
RETURN_MESSAGE_IDLE             equ     $C32C04
CLEAR_MESSAGE_ACTIVE_FLAG       equ     $C32CE6
RETURN_MESSAGE_SEQUENCE         equ     $C32CEC

advance_message_sequence_and_prepare_layout:
                lea.l   MESSAGE_SELECTOR_SEQUENCE.l,a0
                move.b  MESSAGE_SELECTOR_CURSOR.l,d0
                ext.w   d0
                tst.w   2(a0,d0.w)
                beq.b   .selector_sequence_end
                addq.b  #2,MESSAGE_SELECTOR_CURSOR.l
                bra.w   CLEAR_MESSAGE_ACTIVE_FLAG

.selector_sequence_end:
                move.b  #1,MESSAGE_SEQUENCE_MODE.l
                move.w  #$32,MESSAGE_SEQUENCE_STATE.l
                bra.w   .clear_selector_sequence

.glyph_control:
                btst    #0,MESSAGE_RECORD_ATTRIBUTE.l
                bne.b   .advance_glyph_cursor
                subq.b  #1,MESSAGE_DELAY_OR_COUNT.l
                blt.b   .advance_glyph_cursor
                movem.l MESSAGE_LAYOUT_TRIPLET.l,a1-a2/a4
                movem.l a1-a2/a4,MESSAGE_LAYOUT_OUTPUT_TRIPLET.l
                move.b  MESSAGE_GLYPH_REPEAT_COUNT.l,d1
                beq.b   .reload_glyph_counter
                subq.b  #1,d1
                bne.b   .return
.reload_glyph_counter:
                moveq   #2,d1
                bsr.w   MESSAGE_GLYPH_HELPER
.return:
                rts

.advance_glyph_cursor:
                addq.w  #1,a2
                move.b  (a2)+,d0
                ext.w   d0
                asl.w   #2,d0
                move.w  d0,MESSAGE_GLYPH_OFFSET.l
                move.b  (a2),MESSAGE_RECORD_ATTRIBUTE.l
                blt.b   .publish_layout
                addq.w  #1,a2
                move.b  #1,MESSAGE_DELAY_OR_COUNT.l
                move.b  (a2)+,d1
                move.b  d1,d6
                andi.w  #$F0,d6
                lsr.w   #4,d6
                move.w  d6,MESSAGE_LAYOUT_INDEX.l
.publish_layout:
                move.w  #$FFFF,MESSAGE_GLYPH_SENTINEL.l
                adda.l  MESSAGE_LAYOUT_OFFSET.l,a4
                andi.w  #$F,d1
                lea.l   MESSAGE_SELECTOR_SEQUENCE.l,a0
                move.b  MESSAGE_SELECTOR_CURSOR.l,d0
                ext.w   d0
                move.w  (a0,d0.w),d0
                btst    #14,d0
                beq.b   .copy_subtype
                move.b  #2,MESSAGE_RECORD_SUBTYPE.l
                bra.b   .save_layout_triplet
.copy_subtype:
                move.b  d1,MESSAGE_RECORD_SUBTYPE.l
.save_layout_triplet:
                lea.l   $C41066.l,a1
                movem.l a1-a2/a4,MESSAGE_LAYOUT_TRIPLET.l
                bra.w   CONTINUE_GLYPH_COMPOSITOR

.clear_selector_sequence:
                clr.b   MESSAGE_SELECTOR_CURSOR.l
                clr.l   MESSAGE_SELECTOR_SEQUENCE.l
                bra.b   RETURN_MESSAGE_SEQUENCE

.return_sequence:
                move.l  #$1B8,MESSAGE_LAYOUT_OFFSET.l
                tst.w   MESSAGE_SEQUENCE_STATE.l
                ble.b   .clear_active_flag
                subq.w  #1,MESSAGE_SEQUENCE_STATE.l
                bgt.b   .clear_active_flag
                ori.b   #$80,MESSAGE_SEQUENCE_MODE.l
.clear_active_flag:
                clr.b   MESSAGE_ACTIVE_FLAG.l
                rts
