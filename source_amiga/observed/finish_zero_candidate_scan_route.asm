; Byte-exact zero-result candidate route gate $C262FC-$C26303.

                org     $C262FC

finish_zero_candidate_scan_route:
                dc.w    $08A9,$0001,$0003       ; bclr.b #1,$03(a1)
                dc.w    $671A                   ; beq.b $C2631E
