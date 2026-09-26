; Byte-exact observed C1E540 transform continuation $C1E6AC-$C1E6B5.
; It loads the fixed $A00 magnitude bound, subtracts the first saved component
; from D2, and branches directly when the signed delta is nonnegative.

                org     $C1E6AC

prepare_c1e540_first_component_delta:
                move.l  #$a00,d0
                sub.l   d5,d2
                bge.b   $C1E6B8
