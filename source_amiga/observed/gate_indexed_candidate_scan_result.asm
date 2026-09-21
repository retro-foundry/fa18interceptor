; Byte-exact zero-result candidate-scan gate $C26294-$C2629B.

                org     $C26294

gate_indexed_candidate_scan_result:
                dc.w    $0829,$0001,$0020       ; btst.b #1,$20(a1)
                dc.w    $6718                   ; beq.b $C262B4
