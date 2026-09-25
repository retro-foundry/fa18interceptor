; Byte-exact observed candidate bound gates $C275D0-$C275DD.

                org     $C275D0

CANDIDATE_SCAN_REJECT           equ     $C275FC

gate_c275d0_candidate_bounds:
                cmp.w   (a1),d1
                bgt.b   CANDIDATE_SCAN_REJECT
                move.w  $2(a1),d6
                ext.l   d6
                cmp.l   d6,d0
                ble.b   CANDIDATE_SCAN_REJECT
