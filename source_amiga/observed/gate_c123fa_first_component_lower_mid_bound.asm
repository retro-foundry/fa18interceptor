; Byte-exact observed C123FA component route $C124BE-$C124C7.
; It compares signed local -$16 with the observed $00800000 lower mid-bound
; before selecting the next component route.

                org     $C124BE

gate_c123fa_first_component_lower_mid_bound:
                cmpi.l  #$800000,-$16(a6)
                ble.b   $C124D6
