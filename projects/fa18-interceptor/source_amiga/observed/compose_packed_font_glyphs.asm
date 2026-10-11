; Byte-exact packed-font glyph compositor $C327A0-$C32805.
; Runtime traces prove the glyph-stream-to-strided-buffer handoff.

                org     $C327A0

FONT_GLYPH_OFFSET_TABLE         equ     $C3D790
FONT_COMPOSITOR                 equ     $C32806
FONT_ERROR_CODE                 equ     $C4599E
FONT_ERROR_HANDLER              equ     $C06C02

compose_packed_font_glyphs:
                move.l  $0(a0,d5.w),d1
                add.w   d6,d6
.next_glyph:
                move.w  (a1)+,d5
                move.w  (a1)+,d3
                move.b  (a2)+,d4
                move.w  a5,d2
                add.w   d6,d2
                add.w   d5,d2
                blt.b   .skip_glyph
                cmpi.w  #$28,d2
                bge.b   .skip_glyph
                add.w   d6,d5
                ext.l   d5
                add.l   a4,d5
                add.l   d1,d5
                andi.w  #$FF,d4
                subi.w  #$20,d4
                add.w   d4,d4
                lea.l   FONT_GLYPH_OFFSET_TABLE.l,a3
                adda.w  $0(a3,d4.w),a3
                move.l  a3,d4
                swap    d6
                move.w  d6,d2
                swap    d6
                or.w    d3,d2
                btst    #0,d5
                bne.b   .report_odd_destination
                move.l  d1,-(a7)
                move.l  d5,d1
                bsr.w   FONT_COMPOSITOR
                move.l  (a7)+,d1
.skip_glyph:
                dbra    d0,.next_glyph
                rts
.report_odd_destination:
                move.w  #$46,FONT_ERROR_CODE.l
                jsr     FONT_ERROR_HANDLER.l
                bra.b   .skip_glyph
