; Byte-exact observed formatter stage $C32178-$C321D1.
; Reads the selected record byte +$2B, scales its magnitude, converts it to
; packed BCD, and makes a three-character font-render request.

DISPLAY_RECORD_BASE           equ     $C46184
DISPLAY_RECORD_OFFSET         equ     $C458DE
WORKSPACE_INPUT               equ     $C45B1E
PACKED_BCD_CONVERTER          equ     $C25A08
FORMATTER_COORDINATES         equ     $C3198C
FORMATTER_TEXT_SCRATCH        equ     $C457FA
PACKED_FONT_RENDERER          equ     $C3273C

                org     $C32178

format_record_byte_as_three_digits:
                lea.l   DISPLAY_RECORD_BASE.l,a1
                adda.w  DISPLAY_RECORD_OFFSET.l,a1
                move.b  $2B(a1),d0
                ext.w   d0
                asl.w   #8,d0
                bge.s   format_record_byte_positive
                neg.w   d0
format_record_byte_positive:
                ext.l   d0
                divu.w  #$133,d0
                lea.l   FORMATTER_TEXT_SCRATCH+$102,a1
                bsr.w   $C31C20
                bge.s   format_record_byte_submit
                rts
format_record_byte_submit:
                ext.l   d0
                move.l  d0,WORKSPACE_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
                moveq   #2,d0
                lea.l   FORMATTER_COORDINATES(pc),a1
                lea.l   FORMATTER_TEXT_SCRATCH.l,a2
                lea.l   $3(a2),a0
                lea.l   $1B76.w,a4
                lea.l   $1E.w,a5
                move.w  #4,d5
                bra.w   PACKED_FONT_RENDERER
