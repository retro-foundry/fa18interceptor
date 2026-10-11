; Byte-exact observed C1F6F8 transform boundary $C1F838-$C1F843.
; One route returns to the shared transform loop; the adjacent leaf returns
; signed local -$7C in D0 after releasing its frame.

                org     $C1F838

                bra.w   $C1F716

return_c1f6f8_local_word_result:
                move.w  -$7c(a6),d0
                unlk    a6
                rts
