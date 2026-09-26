; Byte-exact observed matrix-side continuation $C135A2-$C135AB.
; It clears the two component locals -$1A and -$1C through a zero D0.

                org     $C135A2

clear_matrix_side_component_locals:
                moveq   #0,d0
                move.w  d0,-$1a(a6)
                move.w  d0,-$1c(a6)
