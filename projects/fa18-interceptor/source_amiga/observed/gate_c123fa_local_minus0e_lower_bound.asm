; Byte-exact observed C123FA component route $C126E8-$C126F1.
; It compares signed local -$0E with the observed $7080 lower bound before
; the adjacent component continuation.

                org     $C126E8

gate_c123fa_local_minus0e_lower_bound:
                cmpi.l  #$7080,-$e(a6)
                blt.b   $C126F6
