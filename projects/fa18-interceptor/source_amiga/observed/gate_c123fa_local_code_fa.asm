; Byte-exact observed C123FA local-code FA gate $C13020-$C13027.

                org     $C13020

gate_c123fa_local_code_fa:
                cmpi.b  #$FA,-$11(a6)
                bne.b   $C13060
