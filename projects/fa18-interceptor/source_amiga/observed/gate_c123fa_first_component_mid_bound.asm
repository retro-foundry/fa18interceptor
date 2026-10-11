; Byte-exact observed C123FA component route $C124A8-$C124B1.
; It compares signed local -$16 against the observed $02000000 mid-bound and
; chooses the following route when it is at most that value.

                org     $C124A8

gate_c123fa_first_component_mid_bound:
                cmpi.l  #$2000000,-$16(a6)
                ble.b   $C124BE
