; Byte-exact observed second byte-delta magnitude gate $C2759E-$C275A3.

                org     $C2759E

CANDIDATE_SCAN_REJECT           equ     $C275FC

gate_c2759e_candidate_second_delta:
                cmpi.b  #1,d2
                bgt.b   CANDIDATE_SCAN_REJECT
