; Byte-exact observed indexed-record triple reset/class gate $C25DD0-$C25DE7.
; The reset route clears the D5/D6/D7 scaled triple and rejoins the shared
; stage; the alternate route begins a high-nibble class-$30 check.

                org     $C25DD0

reset_scaled_indexed_record_triple:
                moveq   #0,d5
                moveq   #0,d6
                moveq   #0,d7
                bra.w   $C25E86

check_indexed_record_class30:
                move.b  $62(a1),d0
                andi.b  #$F0,d0
                cmpi.b  #$30,d0
                bne.b   $C25E10
