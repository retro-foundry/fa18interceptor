; Byte-exact observed entry setup for the scale helper $C2574A-$C25753.
; The branch target is an independently observed internal block.

                org     $C2574A

C25764                         equ     $C25764

initialize_c2574a_scale_helper:
                link.w  a6,#-4
                move.w  d0,-2(a6)
                bra.b   C25764
