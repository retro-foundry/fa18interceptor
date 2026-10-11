; Byte-exact observed C1E540 transform continuation $C1E6B8-$C1E6C1.
; It bounds the first absolute delta against $A00, then subtracts the second
; saved component from D3 and branches when that delta is nonnegative.

                org     $C1E6B8

prepare_c1e540_second_component_delta:
                cmp.l   d0,d2
                bgt.w   $C1EAEC
                sub.l   d6,d3
                bge.b   $C1E6C4
