; Byte-exact observed C1E540 transform continuation $C1E6C4-$C1E6CD.
; It bounds the second absolute delta against $A00, then subtracts the third
; saved component from D4 and branches when that delta is nonnegative.

                org     $C1E6C4

prepare_c1e540_third_component_delta:
                cmp.l   d0,d3
                bgt.w   $C1EAEC
                sub.l   d7,d4
                bge.b   $C1E6D0
