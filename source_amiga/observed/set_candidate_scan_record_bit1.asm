; Byte-exact candidate-scan record bit-1 set route $C262F4-$C262FB.

                org     $C262F4

set_candidate_scan_record_bit1:
                bset.b  #1,$3(a1)
                bra.b   $C2631E
