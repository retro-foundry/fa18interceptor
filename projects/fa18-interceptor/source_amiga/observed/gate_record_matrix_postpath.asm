; Byte-exact observed post-matrix gates $C2D7CA-$C2D7E5.

                org     $C2D7CA

POST_MATRIX_GATE                equ     $C45784
ACTIVE_UPDATE_SELECTOR          equ     $C459B4

gate_record_matrix_postpath:
                tst.b   POST_MATRIX_GATE.l
                beq.w   $C2D8A8
                tst.w   ACTIVE_UPDATE_SELECTOR.l
                bne.b   $C2D7E6
