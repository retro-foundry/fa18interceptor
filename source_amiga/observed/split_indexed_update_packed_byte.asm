; Byte-exact observed indexed-update continuation $C25F4E-$C25F5D.
; It reads record byte $7C, retains its high and low nibbles separately, and
; branches when the low-nibble state is zero.

                org     $C25F4E

split_indexed_update_packed_byte:
                move.b  $7c(a1),d0
                move.b  d0,d1
                andi.b  #$f0,d1
                andi.b  #$f,d0
                beq.b   $C25F90
