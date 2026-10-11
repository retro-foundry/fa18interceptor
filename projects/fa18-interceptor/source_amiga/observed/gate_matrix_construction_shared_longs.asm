; Byte-exact observed matrix-construction continuation $C2E300-$C2E30F.
; Signed tests of two shared long values choose between the adjacent matrix
; parameter-normalization routes; their broader field roles remain open.

                org     $C2E300

MATRIX_STATE_LONG_A            equ     $C45BA6
MATRIX_STATE_LONG_B            equ     $C45BB2

gate_matrix_construction_shared_longs:
                tst.l   MATRIX_STATE_LONG_A.l
                blt.b   $C2E31E
                tst.l   MATRIX_STATE_LONG_B.l
                bge.b   $C2E32E
