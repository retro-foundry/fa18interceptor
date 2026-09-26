; Byte-exact observed context record insertion-scan tail $C1E4F4-$C1E503.
; It marks the selected record terminator, copies its word-$2C payload to the
; output stream, then restores the saved register set and returns.

                org     $C1E4F4

finish_context_record_insertion_scan:
                move.w  #-$1,(a1)
                move.w  $2c(a1),(a4)+
                bra.b   $C1E4C4
                movem.l (sp)+,d0-d6/a0-a5
                rts
