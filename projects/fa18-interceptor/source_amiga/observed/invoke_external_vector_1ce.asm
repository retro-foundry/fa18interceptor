; Byte-exact external-vector wrapper $C53C8C-$C53C9F.

                org     $C53C8C

EXTERNAL_LIBRARY_BASE           equ $C07F64
EXTERNAL_LIBRARY_VECTOR_1CE     equ -$1CE

invoke_external_vector_1ce:
                move.l  a6,-(a7)
                movea.l EXTERNAL_LIBRARY_BASE.l,a6
                movea.l 8(a7),a1
                jsr     EXTERNAL_LIBRARY_VECTOR_1CE(a6)
                movea.l (a7)+,a6
                rts
