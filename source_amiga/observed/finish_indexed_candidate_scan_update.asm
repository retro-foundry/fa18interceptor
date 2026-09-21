; Byte-exact indexed candidate-scan completion $C2627E-$C26283.
; Both observed scan-result routes join this helper and return tail.

                org     $C2627E

GUARD_SELECTED_RECORD_OFFSET     equ $C2651E

finish_indexed_candidate_scan_update:
                bsr.w   GUARD_SELECTED_RECORD_OFFSET
                rts
