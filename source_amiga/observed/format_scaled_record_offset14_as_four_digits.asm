; Byte-exact observed formatter stage $C32260-$C322ED.

DISPLAY_RECORD_BASE           equ     $C46184
DISPLAY_RECORD_OFFSET         equ     $C458DE
FORMATTER_PREVIOUS_WORD       equ     $C4595E
FORMATTER_REPEAT_COUNT        equ     $C4583A
FORMATTER_DISABLE_BYTE        equ     $C457A4
WORKSPACE_INPUT               equ     $C45B1E
PACKED_BCD_CONVERTER          equ     $C25A08
FORMATTER_COORDINATES         equ     $C31A24
FORMATTER_TEXT_SCRATCH        equ     $C457FA
PACKED_FONT_RENDERER          equ     $C32736

                org     $C32260

format_scaled_record_offset14_as_four_digits:
                lea.l   DISPLAY_RECORD_BASE.l,a1
                adda.w  DISPLAY_RECORD_OFFSET.l,a1
                move.l  $14(a1),d0
                subi.l  #$0F000000,d0
                asr.l   #8,d0
                divs.w  #$5999,d0
                addi.w  #$4C4,d0
                cmp.w   FORMATTER_PREVIOUS_WORD.l,d0
                bne.s   format_record_offset14_value_changed
                tst.b   FORMATTER_REPEAT_COUNT.l
                ble.s   format_record_offset14_done
                subq.b  #1,FORMATTER_REPEAT_COUNT.l
                bra.s   format_record_offset14_convert
format_record_offset14_value_changed:
                move.w  d0,FORMATTER_PREVIOUS_WORD.l
                move.b  #2,FORMATTER_REPEAT_COUNT.l
format_record_offset14_convert:
                tst.b   FORMATTER_DISABLE_BYTE.l
                bne.s   format_record_offset14_submit
                ext.l   d0
                move.l  d0,WORKSPACE_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
format_record_offset14_submit:
                moveq   #3,d0
                lea.l   FORMATTER_COORDINATES(pc),a1
                lea.l   FORMATTER_TEXT_SCRATCH.l,a2
                lea.l   $4(a2),a0
                lea.l   $1D84.w,a4
                lea.l   $24.w,a5
                move.w  #4,d5
                movem.l d0/a0-a2/a4-a5,-(a7)
                bsr.w   PACKED_FONT_RENDERER
                movem.l (a7)+,d0/a0-a2/a4-a5
                move.w  #$C,d5
                bra.w   PACKED_FONT_RENDERER
format_record_offset14_done:
                rts
