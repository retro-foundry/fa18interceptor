; Byte-exact helper normalization arm $C1CEC2-$C1CEE3.
                org $C1CEC2
                bsr.w $C1D0A4
                bra.b $C1CECC
                bsr.w $C1D0B6
                movem.l d2-d4,$C45B30.l
                asr.l #8,d2
                asr.l #8,d3
                asr.l #8,d4
                movem.w d2-d4,$C45B2A.l
                bra.b $C1CEEC
