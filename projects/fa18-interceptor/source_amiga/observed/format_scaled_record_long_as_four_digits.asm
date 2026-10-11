; Byte-exact observed formatter stage $C321D2-$C3225F.
; Reads record long +$1C, scales it, converts the resulting signed word to
; packed BCD through $C25A08, then makes two four-character draw submissions.

DISPLAY_RECORD_BASE           equ     $C46184
DISPLAY_RECORD_OFFSET         equ     $C458DE
FORMATTER_PREVIOUS_WORD       equ     $C4595C
FORMATTER_REPEAT_COUNT        equ     $C45839
FORMATTER_DISABLE_BYTE        equ     $C457A4
WORKSPACE_INPUT               equ     $C45B1E
PACKED_BCD_CONVERTER          equ     $C25A08
FORMATTER_COORDINATES         equ     $C31A38
FORMATTER_TEXT_SCRATCH        equ     $C457FA
PACKED_FONT_RENDERER_A        equ     $C32736
PACKED_FONT_RENDERER_B        equ     $C3273C

                org     $C321D2

format_scaled_record_long_as_four_digits:
                lea.l   DISPLAY_RECORD_BASE.l,a1
                adda.w  DISPLAY_RECORD_OFFSET.l,a1
                move.l  $1C(a1),d0
                subi.l  #$10000000,d0
                asr.l   #8,d0
                divs.w  #$7000,d0
                addi.w  #$177,d0
                cmp.w   FORMATTER_PREVIOUS_WORD.l,d0
                bne.s   format_scaled_record_value_changed
                tst.b   FORMATTER_REPEAT_COUNT.l
                ble.s   format_scaled_record_done
                subq.b  #1,FORMATTER_REPEAT_COUNT.l
                bra.s   format_scaled_record_convert
format_scaled_record_value_changed:
                move.w  d0,FORMATTER_PREVIOUS_WORD.l
                move.b  #2,FORMATTER_REPEAT_COUNT.l
format_scaled_record_convert:
                tst.b   FORMATTER_DISABLE_BYTE.l
                bne.s   format_scaled_record_submit
                ext.l   d0
                move.l  d0,WORKSPACE_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
format_scaled_record_submit:
                moveq   #3,d0
                lea.l   FORMATTER_COORDINATES(pc),a1
                lea.l   FORMATTER_TEXT_SCRATCH.l,a2
                lea.l   $4(a2),a0
                lea.l   $1C44.w,a4
                lea.l   $24.w,a5
                move.w  #4,d5
                movem.l d0/a0-a2/a4-a5,-(a7)
                bsr.w   PACKED_FONT_RENDERER_A
                movem.l (a7)+,d0/a0-a2/a4-a5
                move.w  #$C,d5
                bra.w   PACKED_FONT_RENDERER_A
format_scaled_record_done:
                rts
