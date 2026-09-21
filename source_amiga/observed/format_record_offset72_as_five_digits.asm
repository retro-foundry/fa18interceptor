; Byte-exact observed formatter stage $C3212A-$C32177.

DISPLAY_RECORD_BASE           equ     $C46184
DISPLAY_RECORD_OFFSET         equ     $C458DE
WORKSPACE_INPUT               equ     $C45B1E
PACKED_BCD_CONVERTER          equ     $C25A08
FORMATTER_COORDINATES         equ     $C31994
FORMATTER_TEXT_SCRATCH        equ     $C457FB
PACKED_FONT_RENDERER          equ     $C32736

                org     $C3212A

format_record_offset72_as_five_digits:
                lea.l   DISPLAY_RECORD_BASE.l,a1
                adda.w  DISPLAY_RECORD_OFFSET.l,a1
                move.l  $72(a1),d0
                asr.l   #8,d0
                lea.l   $C458F6.l,a1
                bsr.w   $C31C20
                bge.s   format_record_offset72_submit
                rts
format_record_offset72_submit:
                ext.l   d0
                move.l  d0,WORKSPACE_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
                moveq   #4,d0
                lea.l   FORMATTER_COORDINATES(pc),a1
                lea.l   FORMATTER_TEXT_SCRATCH.l,a2
                lea.l   $5(a2),a0
                lea.l   $1CA4.w,a4
                lea.l   $C.w,a5
                move.w  #0,d5
                bra.w   PACKED_FONT_RENDERER
