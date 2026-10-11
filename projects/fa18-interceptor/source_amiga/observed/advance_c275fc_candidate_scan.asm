; Byte-exact observed rejection/loop-control tail $C275FC-$C27615.

                org     $C275FC

CANDIDATE_SCAN_FINISH           equ     $C27968
CANDIDATE_SCAN_NEXT             equ     $C27534

advance_c275fc_candidate_scan:
                move.w  -$6(a6),d0
                cmp.w   -$8(a6),d0
                ble.w   CANDIDATE_SCAN_FINISH
                cmpi.w  #$a,d5
                bge.w   CANDIDATE_SCAN_FINISH
                bra.w   CANDIDATE_SCAN_NEXT
