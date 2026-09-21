; Byte-exact indexed candidate-scan gate $C25FEE-$C25FF5.

                org     $C25FEE

gate_indexed_candidate_scan:
                dc.w    $0829,$0002,$0000       ; btst.b #2,$00(a1)
                dc.w    $6708                   ; beq.b $C25FFE
