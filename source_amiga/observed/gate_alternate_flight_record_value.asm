; Byte-exact alternate record value gate $C1CEE4-$C1CF35.
                org $C1CEE4
                movem.l d2-d4,$C45B30.l
                move.w $4(a0),d1
                ext.l d1
                asl.l d0,d1
                cmpi.l #$400,d1
                bge.b $C1CF36
                cmpi.l #$100,d1
                blt.b .scale
                move.w $C458DA.l,d0
                move.w d7,d1
                btst #8,d1
                bne.b .bit_set
                andi.w #3,d0
                beq.b .scale
                bra.b $C1CF36
.bit_set:       andi.w #3,d0
                cmpi.w #2,d0
                bne.b $C1CF36
.scale:         movem.w $C45B2A.l,d2-d4
                jsr $C1D91A.l
                move.w d1,$4(a0)
