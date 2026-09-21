; Byte-exact adjacent library-vector wrappers $C53F30-$C53F67.
; Library identity and individual LVO meanings remain unassigned.
                org     $C53F30

LIBRARY_BASE_POINTER            equ $C182CA
LIBRARY_LVO_DE                  equ -$DE
LIBRARY_LVO_E4                  equ -$E4
LIBRARY_LVO_168                 equ -$168

invoke_library_lvo_de_with_a1_argument:
                move.l  a6,-(sp)
                movea.l 8(sp),a1
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_DE(a6)
                movea.l (sp)+,a6
                rts

invoke_library_lvo_e4:
                move.l  a6,-(sp)
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_E4(a6)
                movea.l (sp)+,a6
                rts

invoke_library_lvo_168_with_a1_argument:
                move.l  a6,-(sp)
                movea.l 8(sp),a1
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_168(a6)
                movea.l (sp)+,a6
                rts
