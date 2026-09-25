; Byte-exact observed basic block $C15218-$C15223.

                org     $C15218

RECORD_SCAN_FLAGS               equ     $C458C6

gate_c15218_record_scan_flag:
                move.w  RECORD_SCAN_FLAGS.l,d0
                btst    #3,d0
                bne.b   $C1522E
