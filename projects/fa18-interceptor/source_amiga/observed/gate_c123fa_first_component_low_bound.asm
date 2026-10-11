; Byte-exact observed C123FA component route $C124D6-$C124DF.
; It compares signed local -$16 with the observed $00200000 low bound before
; selecting the next component route.

                org     $C124D6

gate_c123fa_first_component_low_bound:
                cmpi.l  #$200000,-$16(a6)
                ble.b   $C124EE
