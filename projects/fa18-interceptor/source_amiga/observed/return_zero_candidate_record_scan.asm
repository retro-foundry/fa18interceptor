; Byte-exact zero-result candidate-record scan return $C26EB8-$C26EBD.

                org     $C26EB8

return_zero_candidate_record_scan:
                moveq   #0,d0
                unlk    a6
                rts
