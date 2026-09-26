; Byte-exact observed system-vector jump-table entries $C000FC-$C0010D.
; These direct absolute jump stubs enter services outside the observed game
; code; their service identities are left unassigned.

                org     $C000FC

jump_system_vector_table_000fc:
                jmp     $FC1C18.l
                jmp     $FC1BEA.l
                jmp     $FC1B70.l
