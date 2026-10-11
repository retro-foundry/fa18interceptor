; Byte-exact indexed-record matrix dispatch call $C25D9C-$C25DA5.

                org     $C25D9C

RUN_RECORD_MATRIX_DISPATCH       equ $C2D408

run_indexed_record_matrix_dispatch:
                move.l  a1,-(a7)
                jsr     RUN_RECORD_MATRIX_DISPATCH.l
                movea.l (a7)+,a1
