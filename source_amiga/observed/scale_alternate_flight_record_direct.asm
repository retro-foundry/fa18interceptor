; Byte-exact direct scale arm $C1CEA4-$C1CEC1.
                org $C1CEA4
                btst #6,d7
                bne.w $C1CEC2
                btst #4,d7
                bne.b $C1CEC8
                movem.w d2-d4,$C45B2A.l
                asl.l #8,d2
                asl.l #8,d3
                asl.l #8,d4
                bra.b $C1CEE4
