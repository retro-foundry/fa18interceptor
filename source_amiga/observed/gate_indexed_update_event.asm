; Byte-exact indexed-update event gate $C25B78-$C25B7F.

                org     $C25B78

INDEXED_UPDATE_EVENT_BYTE        equ $C457AE

gate_indexed_update_event:
                or.b    INDEXED_UPDATE_EVENT_BYTE.l,d0
                bne.b   $C25B64
