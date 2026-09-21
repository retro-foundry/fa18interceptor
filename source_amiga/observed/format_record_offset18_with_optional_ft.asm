; Byte-exact observed formatter stage $C3201A-$C32129.
; Converts selected-record long +$18, tracks redraw state, and formats six
; packed-decimal characters. One alternate submission appends literal "FT".

DISPLAY_RECORD_BASE           equ     $C46184
DISPLAY_RECORD_OFFSET         equ     $C458DE
FORMATTER_OVERRIDE_LONG       equ     $C4565C
FORMATTER_MODE_BYTE           equ     $C45785
FORMATTER_ALT_MODE_BYTE       equ     $C457D9
FORMATTER_FLAGS_BYTE          equ     $C458CD
FORMATTER_ACTIVITY_BYTE       equ     $C45837
FORMATTER_PREVIOUS_LONG       equ     $C45900
WORKSPACE_INPUT               equ     $C45B1E
PACKED_BCD_CONVERTER          equ     $C25A08
FORMATTER_COORDINATES         equ     $C31928
FORMATTER_TEXT_SCRATCH        equ     $C457FE
PACKED_FONT_RENDERER          equ     $C3271A

                org     $C3201A

format_record_offset18_with_optional_ft:
                tst.b   $C457A4.l
                beq.s   format_record_offset18_from_record
                move.l  FORMATTER_OVERRIDE_LONG.l,d0
                cmpi.l  #$1869F,d0
                ble.s   format_record_offset18_filter
                move.l  #$1869F,d0
                bra.s   format_record_offset18_filter
format_record_offset18_from_record:
                lea.l   DISPLAY_RECORD_BASE.l,a0
                adda.w  DISPLAY_RECORD_OFFSET.l,a0
                move.l  $18(a0),d0
                asr.l   #7,d0
                asr.l   #3,d0
                move.l  d0,d1
                add.l   d0,d0
                add.l   d0,d0
                add.l   d1,d0
format_record_offset18_filter:
                tst.b   FORMATTER_MODE_BYTE.l
                beq.s   format_record_offset18_normal_mode
                tst.b   FORMATTER_ALT_MODE_BYTE.l
                beq.s   format_record_offset18_return
                btst    #6,FORMATTER_FLAGS_BYTE.l
                beq.s   format_record_offset18_return
                bra.s   format_record_offset18_store
format_record_offset18_normal_mode:
                tst.b   FORMATTER_ACTIVITY_BYTE.l
                bgt.s   format_record_offset18_store
                move.l  FORMATTER_PREVIOUS_LONG.l,d2
                blt.s   format_record_offset18_clear_previous_sign
                cmp.l   d2,d0
                beq.s   format_record_offset18_return
                btst    #0,$C458DB.l
                bne.s   format_record_offset18_return
                bra.s   format_record_offset18_store
format_record_offset18_return:
                rts
format_record_offset18_clear_previous_sign:
                andi.l  #$7FFFFFFF,FORMATTER_PREVIOUS_LONG.l
                andi.l  #$7FFFFFFF,d2
                move.l  d2,d0
                bra.s   format_record_offset18_convert
format_record_offset18_store:
                move.l  d0,FORMATTER_PREVIOUS_LONG.l
                ori.l   #$80000000,FORMATTER_PREVIOUS_LONG.l
format_record_offset18_convert:
                move.l  d0,WORKSPACE_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
                moveq   #5,d0
                lea.l   FORMATTER_COORDINATES(pc),a1
                lea.l   FORMATTER_TEXT_SCRATCH.l,a2
                lea.l   $6(a2),a0
                tst.b   FORMATTER_MODE_BYTE.l
                beq.s   format_record_offset18_normal_layout
                lea.l   $1CDA.w,a4
                lea.l   $1A.w,a5
                bra.s   format_record_offset18_layout_ready
format_record_offset18_normal_layout:
                lea.l   $18CE.w,a4
                lea.l   $1E.w,a5
format_record_offset18_layout_ready:
                move.w  #$FCA,d6
                tst.b   FORMATTER_MODE_BYTE.l
                bne.s   format_record_offset18_append_unit
                move.w  #4,d5
                bra.w   $C32740
format_record_offset18_append_unit:
                move.w  #0,d5
                addq.w  #2,d0
                move.b  #'F',$6(a2)
                move.b  #'T',$7(a2)
                movem.l d0/a0-a2/a4-a5,-(a7)
                bsr.w   PACKED_FONT_RENDERER
                movem.l (a7)+,d0/a0-a2/a4-a5
                move.w  #$F3A,d6
                move.w  #$C,d5
                bra.w   PACKED_FONT_RENDERER
