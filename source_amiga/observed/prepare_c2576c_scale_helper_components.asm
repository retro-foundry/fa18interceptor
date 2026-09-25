; Byte-exact observed component preparation $C2576C-$C25775.

                org     $C2576C

C25772                         equ     $C25772
C25778                         equ     $C25778

prepare_c2576c_scale_helper_components:
                move.w  d5,d2
                bge.b   C25772
                neg.w   d2
                move.w  d6,d3
                bge.b   C25778
