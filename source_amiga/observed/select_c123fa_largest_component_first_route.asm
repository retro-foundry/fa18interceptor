; Byte-exact observed C123FA largest-component selection $C12440-$C12455.
; Following absolute-value normalization, this first route retains the first
; component when it exceeds the other two; otherwise it selects the third or
; falls into the adjacent second-component route.

                org     $C12440

select_c123fa_largest_component_first_route:
                move.l  $10(a6),d0
                cmp.l   $14(a6),d0
                ble.b   $C1245E
                cmp.l   $18(a6),d0
                ble.b   $C12456
                move.l  d0,-$16(a6)
                bra.b   $C12474
