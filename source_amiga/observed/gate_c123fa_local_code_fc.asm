; Byte-exact observed C123FA local-code FC gate $C12FD4-$C12FDB.

                org     $C12FD4

gate_c123fa_local_code_fc:
                cmpi.b  #$FC,-$11(a6)
                bne.b   $C12FFE
