; Byte-exact observed candidate-stream continuation $C27388-$C27393.
; It retains descriptor word +2 in D7 and tests its $4000 flag before the
; following candidate route.

                org     $C27388

gate_candidate_stream_bit14:
                move.w  $2(a4),d3
                move.w  d3,d7
                andi.w  #$4000,d3
                beq.b   $C2739A
