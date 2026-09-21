; Byte-exact indexed-update transform call $C25E24-$C25E2D.

                org     $C25E24

RUN_INDEXED_UPDATE_TRANSFORM     equ $C149BE

run_indexed_update_transform:
                move.l  a1,-(a7)
                jsr     RUN_INDEXED_UPDATE_TRANSFORM.l
                movea.l (a7)+,a1
