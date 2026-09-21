; Byte-exact adjacent library-vector wrappers $C53FB0-$C53FCF.
; The library identity and LVO operation meanings remain unassigned.
                org     $C53FB0

LIBRARY_BASE_POINTER            equ $C182CA
LIBRARY_LVO_1C8                 equ -$1C8
LIBRARY_LVO_1CE                 equ -$1CE

invoke_library_lvo_1c8:
                move.l  a6,-(sp)
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_1C8(a6)
                movea.l (sp)+,a6
                rts

invoke_library_lvo_1ce:
                move.l  a6,-(sp)
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_1CE(a6)
                movea.l (sp)+,a6
                rts
