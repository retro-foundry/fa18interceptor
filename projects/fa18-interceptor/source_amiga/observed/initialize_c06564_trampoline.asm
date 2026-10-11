; Byte-exact observed trampoline entry $C06564-$C0656D.

                org     $C06564

C06598                         equ     $C06598

initialize_c06564_trampoline:
                subq.l  #8,sp
                pea     $FE46D2.l
                bra.b   C06598
