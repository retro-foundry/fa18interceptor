; Byte-exact observed indexed-update continuation $C25F90-$C25F9B.
; It copies the packed byte's low nibble to D0, then routes from the signed
; state of the unmasked companion byte in D1.

                org     $C25F90

gate_indexed_update_packed_byte_sign:
                move.b  d1,d0
                andi.b  #$f,d0
                tst.b   d1
                blt.b   $C25FA2
                ble.b   $C25FC2
