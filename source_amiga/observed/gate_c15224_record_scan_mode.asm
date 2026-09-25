; Byte-exact observed basic block $C15224-$C1522D.

                org     $C15224

RECORD_SCAN_AUXILIARY_FLAG      equ     $C4588B
RECORD_SCAN_ADVANCE_PATH        equ     $C153BC

gate_c15224_record_scan_mode:
                tst.b   RECORD_SCAN_AUXILIARY_FLAG.l
                beq.w   RECORD_SCAN_ADVANCE_PATH
