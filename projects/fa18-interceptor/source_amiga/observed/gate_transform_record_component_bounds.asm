; Byte-exact observed transform-record component-bound gate $C1E6D0-$C1E6E5.
; It rejects the third absolute component above the $A00 bound, then dispatches
; the following transform route from descriptor bits 6 and 4.

                org     $C1E6D0

gate_transform_record_component_bounds:
                cmp.l   d0,d4
                bgt.w   $C1EAEC
                move.w  (a1),d1
                btst    #6,d1
                bne.b   $C1E6FE
                btst    #4,d1
                bne.w   $C1E766
