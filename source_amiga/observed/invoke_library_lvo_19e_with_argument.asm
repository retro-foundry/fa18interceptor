; Byte-exact observed library-vector wrapper $C53F9C-$C53FAF.
; The training parent pushes a longword argument before this call.

                org     $C53F9C

LIBRARY_BASE_POINTER            equ $C182CA
LIBRARY_VECTOR_19E              equ -$19E

invoke_library_lvo_19e_with_argument:
                move.l  a6,-(a7)
                move.l  8(a7),d0
                movea.l LIBRARY_BASE_POINTER.l,a6
                jsr     LIBRARY_VECTOR_19E(a6)
                movea.l (a7)+,a6
                rts
