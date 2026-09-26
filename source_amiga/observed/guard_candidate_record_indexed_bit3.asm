; Byte-exact indexed candidate-record bit-3 guard $C26F30-$C26F37.

                org     $C26F30

guard_candidate_record_indexed_bit3:
                btst.b  #3,$1(a0,d0.w)
                beq.b   $C26F00
