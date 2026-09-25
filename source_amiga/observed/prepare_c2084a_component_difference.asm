; Byte-exact observed first component-difference setup $C2084A-$C20861.

                org     $C2084A

COMPONENT_DIFFERENCE_HELPER     equ     $C2574A

prepare_c2084a_component_difference:
                move.w  (a3)+,d5
                move.w  (a3)+,d7
                sub.w   (a3)+,d5
                sub.w   (a3)+,d7
                neg.w   d5
                neg.w   d7
                clr.w   d6
                move.w  #$100,d0
                jsr     COMPONENT_DIFFERENCE_HELPER.l
