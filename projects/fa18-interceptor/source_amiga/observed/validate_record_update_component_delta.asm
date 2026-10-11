; Byte-exact observed record-update continuation $C240BE-$C240C7.
; It subtracts D0 from D3, normalizes that delta to nonnegative magnitude,
; and compares it with D6 before the component-bound route.

                org     $C240BE

validate_record_update_component_delta:
                sub.l   d0,d3
                bge.b   $C240C4
                neg.l   d3
                cmp.l   d6,d3
                bgt.b   $C240E2
