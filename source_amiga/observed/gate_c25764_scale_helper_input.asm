; Byte-exact observed input gate in the scale helper $C25764-$C25769.

                org     $C25764

C2576C                         equ     $C2576C
C257D4                         equ     $C257D4

gate_c25764_scale_helper_input:
                tst.w   d0
                beq.b   C257D4
                bgt.b   C2576C
