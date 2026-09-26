; Byte-exact observed C123FA component route $C126F6-$C126FF.
; It compares signed local -$12 with the observed $7080 lower bound before
; the adjacent component continuation.

                org     $C126F6

gate_c123fa_local_minus12_lower_bound:
                cmpi.l  #$7080,-$12(a6)
                blt.b   $C12704
