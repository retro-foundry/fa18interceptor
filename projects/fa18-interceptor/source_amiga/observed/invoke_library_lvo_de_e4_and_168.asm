; Byte-exact graphics.library LoadView/WaitBlit/InitView wrappers $C53F30-$C53F67.
                org     $C53F30

GFX_BASE_POINTER                equ $C182CA
LOAD_VIEW_LVO                   equ -$DE
WAIT_BLIT_LVO                   equ -$E4
INIT_VIEW_LVO                   equ -$168

invoke_graphics_load_view_with_a1:
                move.l  a6,-(sp)
                movea.l 8(sp),a1
                movea.l GFX_BASE_POINTER.l,a6
                jsr     LOAD_VIEW_LVO(a6)
                movea.l (sp)+,a6
                rts

invoke_graphics_wait_blit:
                move.l  a6,-(sp)
                movea.l GFX_BASE_POINTER.l,a6
                jsr     WAIT_BLIT_LVO(a6)
                movea.l (sp)+,a6
                rts

invoke_graphics_init_view_with_a1:
                move.l  a6,-(sp)
                movea.l 8(sp),a1
                movea.l GFX_BASE_POINTER.l,a6
                jsr     INIT_VIEW_LVO(a6)
                movea.l (sp)+,a6
                rts
