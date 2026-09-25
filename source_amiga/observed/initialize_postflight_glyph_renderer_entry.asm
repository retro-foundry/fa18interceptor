; Byte-exact postflight glyph-renderer entries $C3278C-$C3279F.

                org     $C3278C

POSTFLIGHT_GLYPH_POINTER_BLOCK  equ     $C456B6

initialize_postflight_glyph_renderer_entry:
                swap    d6
                move.w  #0,d6
                moveq   #0,d7
join_postflight_glyph_renderer:
                adda.l  d7,a4
                move.w  #$142,d7
                movea.l POSTFLIGHT_GLYPH_POINTER_BLOCK.l,a0

