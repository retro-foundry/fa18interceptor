; Byte-exact zero-result candidate state gate $C262E2-$C262F3.

                org     $C262E2

CANDIDATE_SCAN_STATE             equ $C4589F

gate_zero_candidate_scan_state:
                dc.w    $0829,$0001,$0000       ; btst.b #1,$00(a1)
                bne.w   $C2625E
                tst.b   CANDIDATE_SCAN_STATE.l
                dc.w    $6708                   ; beq.b $C262FC
