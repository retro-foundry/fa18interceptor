; Byte-exact external-vector wrapper $C53C78-$C53C8B.
; It passes the stack argument in A1 to the library vector at -$1C8 and
; preserves the caller's A6.  Callers use it during activity input setup.

                org     $C53C78

EXTERNAL_LIBRARY_BASE           equ     $C07F64
EXTERNAL_LIBRARY_VECTOR_1C8     equ     -$1C8

invoke_external_vector_1c8:
                move.l  a6,-(a7)
                movea.l EXTERNAL_LIBRARY_BASE.l,a6
                movea.l 8(a7),a1
                jsr     EXTERNAL_LIBRARY_VECTOR_1C8(a6)
                movea.l (a7)+,a6
                rts
