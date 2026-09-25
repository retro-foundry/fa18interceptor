; Byte-exact packed-font text prefix $C32740-$C3278B.
; Its glyph iteration and compositor continuation are reconstructed separately.

                org     $C32740

PACKED_FONT_RENDER_LANE         equ     $C45986
PACKED_FONT_RENDER_OFFSET       equ     $C45918
PACKED_FONT_VALUE               equ     $C45B22
PACKED_FONT_GLYPH_ENTRY         equ     $C32794

format_packed_font_text_prefix:
                swap    d6
                move.w  PACKED_FONT_RENDER_LANE.l,d6
                move.l  PACKED_FONT_RENDER_OFFSET.l,d7
                move.w  d0,d2
                moveq   #0,d4
                move.l  PACKED_FONT_VALUE.l,d3
.emit_packed_nibble:
                move.w  d3,d1
                andi.w  #$F,d1
                addi.w  #$30,d1
                cmpi.w  #$39,d1
                ble.b   .store_character
                addq.w  #7,d1
.store_character:
                move.b  d1,-(a0)
                lsr.l   #4,d3
                dbra    d2,.emit_packed_nibble
                tst.b   d4
                bne.b   PACKED_FONT_GLYPH_ENTRY
                move.w  d0,d2
                subq.w  #1,d2
.blank_leading_zeroes:
                cmpi.b  #$30,(a0)+
                bne.b   PACKED_FONT_GLYPH_ENTRY
                move.b  #$20,-1(a0)
                dbra    d2,.blank_leading_zeroes
                bra.b   PACKED_FONT_GLYPH_ENTRY

