; Byte-exact observed C123FA local-code FD gate $C12FA8-$C12FAF.

                org     $C12FA8

gate_c123fa_local_code_fd:
                cmpi.b  #$FD,-$11(a6)
                bne.b   $C12FD4
