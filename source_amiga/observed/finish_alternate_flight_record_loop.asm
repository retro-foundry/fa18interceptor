; Byte-exact alternate record-loop tail $C1CF36-$C1CFC9.
                org $C1CF36
                btst #6,d7
                bne.b .dispatch
                btst #4,d7
                bne.b .dispatch
                move.w $8(a0),$C45ABA.l
                bge.b .dispatch
                subq.b #1,$6(a0)
                bge.b $C1CFBE
                move.b $C458BC.l,$6(a0)
.dispatch:      subq.b #1,$7(a0)
                move.b $7(a0),$C458BD.l
                move.w $4(a0),d1
                move.w d1,$C45B40.l
                andi.w #$ff00,d7
                add.w d7,d7
                move.w d7,$C459B6.l
                cmp.w $C458DE.l,d7
                bne.b .load
                tst.b $C45785.l
                bne.b .store
                move.w #$7fff,d1
.store:         move.w d1,$C45B42.l
.load:          movea.l (a1)+,a2
                movea.l (a1)+,a0
                move.l (a1)+,$C45A36.l
                move.l (a1)+,$C45A3A.l
                jsr (a2)
                lea $C4F6CA.l,a0
                move.w $C459AA.l,d1
                tst.w d0
                bgt.b .positive
                moveq #-1,d0
.positive:      move.w d0,$14(a0,d1.w)
                addi.w #$18,$C459AA.l
                bra.w $C1CE42
                addi.w #$18,$C459AA.l
                bra.w $C1CE42
