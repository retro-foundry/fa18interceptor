; Byte-exact observed zero-and-return block $C148D8-$C148E1.

                org     $C148D8

return_c148d8_stack_compare:
                movea.l 8(a6),a0
                clr.w   (a0)
                unlk    a6
                rts
