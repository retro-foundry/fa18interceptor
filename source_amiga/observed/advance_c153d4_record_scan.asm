; Byte-exact observed loop advance $C153D4-$C153DB.

                org     $C153D4

RECORD_SCAN_LOOP_HEAD           equ     $C151C2

advance_c153d4_record_scan:
                addq.w  #2,-$10(a6)
                bra.w   RECORD_SCAN_LOOP_HEAD
