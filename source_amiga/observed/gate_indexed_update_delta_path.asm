; Byte-exact indexed-update delta-path gate $C25E2E-$C25E3B.

                org     $C25E2E

gate_indexed_update_delta_path:
                movem.l $3E(a1),d5-d7
                dc.w    $0829,$0004,$0000       ; btst.b #4,$00(a1)
                dc.w    $6608                   ; bne.b $C25E44
