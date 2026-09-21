; Byte-exact external-vector wrapper $C53C08-$C53C1B.

                org     $C53C08

EXTERNAL_LIBRARY_BASE           equ $C07F64
EXTERNAL_LIBRARY_VECTOR_174     equ -$174

invoke_external_vector_174:
                move.l  a6,-(a7)
                movea.l EXTERNAL_LIBRARY_BASE.l,a6
                movea.l 8(a7),a0
                jsr     EXTERNAL_LIBRARY_VECTOR_174(a6)
                movea.l (a7)+,a6
                rts
