; Byte-exact zero-result candidate flag clear/gate $C262C2-$C262CD.

                org     $C262C2

clear_zero_candidate_scan_flag:
                dc.w    $08A9,$0007,$0000       ; bclr.b #7,$00(a1)
                dc.w    $0C02,$0000             ; cmpi.b #0,d2
                dc.w    $6614                   ; bne.b $C262E2
