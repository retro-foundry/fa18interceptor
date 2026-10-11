; Byte-exact graphics.library WaitBOVP wrapper $C53F88-$C53F9B.
                org     $C53F88

GFX_BASE_POINTER                equ $C182CA
WAIT_BOVP_LVO                   equ -$192

invoke_graphics_wait_bovp_with_viewport:
                move.l  a6,-(sp)
                movea.l 8(sp),a0
                movea.l GFX_BASE_POINTER.l,a6
                jsr     WAIT_BOVP_LVO(a6)
                movea.l (sp)+,a6
                rts
