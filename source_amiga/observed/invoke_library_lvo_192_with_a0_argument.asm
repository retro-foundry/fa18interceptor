; Byte-exact library-vector wrapper $C53F88-$C53F9B.
; The library identity and LVO operation meaning remain unassigned.
                org     $C53F88

LIBRARY_BASE_POINTER            equ $C182CA
LIBRARY_LVO_192                 equ -$192

invoke_library_lvo_192_with_a0_argument:
                move.l  a6,-(sp)
                movea.l 8(sp),a0
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_LVO_192(a6)
                movea.l (sp)+,a6
                rts
