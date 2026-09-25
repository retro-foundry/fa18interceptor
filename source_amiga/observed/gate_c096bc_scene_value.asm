; Byte-exact observed condition and dispatch gate $C096BC-$C096C9.

                org     $C096BC

C096B8                         equ     $C096B8
C096D6                         equ     $C096D6
C45A66                         equ     $C45A66

gate_c096bc_scene_value:
                cmpi.l  #-$380000,C45A66.l
                blt.b   C096B8
                bra.b   C096D6
