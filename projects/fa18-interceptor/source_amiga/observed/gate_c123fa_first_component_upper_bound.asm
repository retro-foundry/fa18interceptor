; Byte-exact observed C123FA component route $C12474-$C1247D.
; It compares signed local -$16 against the observed $10000000 bound and
; chooses the first upper-bound continuation when it is at most that value.

                org     $C12474

gate_c123fa_first_component_upper_bound:
                cmpi.l  #$10000000,-$16(a6)
                ble.b   $C124A8
