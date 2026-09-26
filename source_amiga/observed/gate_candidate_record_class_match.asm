; Byte-exact observed candidate-filter continuation $C26F5A-$C26F67.
; It checks the local byte at -$22 against zero and against the masked
; candidate class in D7 before choosing the matching continuation.

                org     $C26F5A

gate_candidate_record_class_match:
                move.b  -$22(a6),d1
                dc.w    $0c01,$0000             ; cmpi.b #0,d1
                beq.b   $C26F68
                cmp.b   d7,d1
                beq.b   $C26F76
