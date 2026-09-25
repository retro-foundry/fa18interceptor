; Byte-exact observed candidate-table selection block $C2755C-$C27575.

                org     $C2755C

CANDIDATE_INDEX_TABLE           equ     $C4D790
CANDIDATE_DESCRIPTOR_TABLE      equ     $C4D7BC
CANDIDATE_SCAN_REJECT           equ     $C275FC

select_c2755c_candidate_scan_tables:
                addq.w  #1,d5
                lea.l   CANDIDATE_INDEX_TABLE.l,a0
                lea.l   CANDIDATE_DESCRIPTOR_TABLE.l,a1
                move.w  d5,d0
                ; ASL.W #2,D0; preserve the observed compact encoding.
                dc.w    $e540
                move.l  $0(a0,d0.w),d1
                blt.w   CANDIDATE_SCAN_REJECT
