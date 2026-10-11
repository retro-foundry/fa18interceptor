; Byte-exact observed function return and indexed-sequence enable gate
; $C25598-$C255A1.  The following helper returns until its byte enable is set.

                org     $C25598

return_prior_indexed_helper:
                rts

gate_indexed_sequence_enable:
                move.b  $C457C0.l,d0
                beq.b   return_prior_indexed_helper
