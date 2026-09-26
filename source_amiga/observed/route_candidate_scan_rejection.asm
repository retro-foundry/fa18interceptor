; Byte-exact candidate scan rejection route $C2744C-$C2744F.
; Several pair and plane-side guards rejoin the next candidate at $C270B8.

                org     $C2744C

route_candidate_scan_rejection:
                dc.w    $6000,$FC68             ; bra.w $C270B8
