; Byte-exact observed first byte-delta magnitude gate $C2758C-$C27591.

                org     $C2758C

CANDIDATE_SCAN_REJECT           equ     $C275FC

gate_c2758c_candidate_delta:
                cmpi.b  #1,d2
                bgt.b   CANDIDATE_SCAN_REJECT
