; Byte-exact observed system-vector jump-table entries $C001EC-$C00203.
; These are direct absolute jump stubs; the ROM/vector service semantics are
; not assigned from this binary slice alone.

                org     $C001EC

jump_system_vector_table_001ec:
                jmp     $FC1F9C.l
                jmp     $FC1F96.l
                jmp     $FC1436.l
                jmp     $FC1428.l
