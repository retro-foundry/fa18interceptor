; Byte-exact observed signed-divide entry $C52EC8-$C52ED1.

                org     $C52EC8

C52F02                         equ     $C52F02

initialize_c52ec8_signed_divide:
                movem.l d2-d5,-(sp)
                move.l  d1,d5
                beq.b   C52F02
