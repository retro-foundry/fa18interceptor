; Byte-exact observed signed-divide numerator gates $C52ED4-$C52ED9.

                org     $C52ED4

C52EDC                         equ     $C52EDC
C52F00                         equ     $C52F00

gate_c52ed4_signed_divide_input:
                move.l  d0,d4
                beq.b   C52F00
                bpl.b   C52EDC
