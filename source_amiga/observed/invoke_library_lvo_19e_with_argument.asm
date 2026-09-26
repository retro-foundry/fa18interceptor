; Byte-exact graphics.library FreeSprite wrapper $C53F9C-$C53FAF.
; The training parent pushes a longword argument before this call.

                org     $C53F9C

GFX_BASE_POINTER                equ $C182CA
FREE_SPRITE_LVO                 equ -$19E

invoke_graphics_free_sprite_with_d0:
                move.l  a6,-(a7)
                move.l  8(a7),d0
                movea.l GFX_BASE_POINTER.l,a6
                jsr     FREE_SPRITE_LVO(a6)
                movea.l (a7)+,a6
                rts
