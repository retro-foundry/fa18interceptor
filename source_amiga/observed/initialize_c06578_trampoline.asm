; Byte-exact observed trampoline entry $C06578-$C06581.

                org     $C06578

C06598                         equ     $C06598

initialize_c06578_trampoline:
                subq.l  #8,sp
                pea     $FE48FE.l
                bra.b   C06598
