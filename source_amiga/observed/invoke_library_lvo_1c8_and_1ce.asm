; Byte-exact graphics.library OwnBlitter/DisownBlitter wrappers $C53FB0-$C53FCF.
                org     $C53FB0

GFX_BASE_POINTER                equ $C182CA
OWN_BLITTER_LVO                 equ -$1C8
DISOWN_BLITTER_LVO              equ -$1CE

invoke_graphics_own_blitter:
                move.l  a6,-(sp)
                movea.l GFX_BASE_POINTER.l,a6
                jsr     OWN_BLITTER_LVO(a6)
                movea.l (sp)+,a6
                rts

invoke_graphics_disown_blitter:
                move.l  a6,-(sp)
                movea.l GFX_BASE_POINTER.l,a6
                jsr     DISOWN_BLITTER_LVO(a6)
                movea.l (sp)+,a6
                rts
