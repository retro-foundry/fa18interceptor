; Byte-exact indexed candidate-record scan handoff $C25FFE-$C2601B.

                org     $C25FFE

RUN_CANDIDATE_RECORD_SCAN        equ $C26EBE

run_indexed_candidate_record_scan:
                movem.l $14(a1),d2-d4
                movem.l (a7),d5-d7
                sub.l   d5,d2
                sub.l   d6,d3
                sub.l   d7,d4
                jsr     RUN_CANDIDATE_RECORD_SCAN.l
                movem.l (a7)+,d5-d7/a1
                beq.w   $C26294
