; Byte-exact observed C123FA local-code FB gate $C12FFE-$C13005.

                org     $C12FFE

gate_c123fa_local_code_fb:
                cmpi.b  #$FB,-$11(a6)
                bne.b   $C13020
