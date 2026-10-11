; Byte-exact observed trampoline entry $C0655A-$C06563.

                org     $C0655A

C06598                         equ     $C06598

initialize_c0655a_trampoline:
                subq.l  #8,sp
                pea     $FE46BE.l
                bra.b   C06598
