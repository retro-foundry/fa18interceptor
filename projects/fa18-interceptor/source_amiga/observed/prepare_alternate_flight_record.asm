; Byte-exact alternate record-route setup $C1CE38-$C1CEA3.
                org $C1CE38
ALT_OFFSET equ $C459AA
ALT_SOURCE_OFFSET equ $C459B0
ALT_LIST equ $C4F6CA
ALT_SELECTOR equ $C4585B
ALT_SHIFT equ $C45AB8
ALT_KIND equ $C459B4
ALT_WORK equ $C45932
ALT_READY equ $C458BB
ALT_GUARD equ $C45A66

prepare_alternate_flight_record:
                move.w ALT_SOURCE_OFFSET.l,ALT_OFFSET.l
                lea ALT_LIST.l,a0
                move.w ALT_OFFSET.l,d0
                adda.w d0,a0
                move.w (a0)+,d7
                cmpi.w #$ffff,d7
                beq.w $C1CFD6
                move.b d7,ALT_SELECTOR.l
                move.b d7,d0
                andi.w #$000f,d0
                move.w d0,ALT_SHIFT.l
                movea.l (a0)+,a1
                tst.w (a1)
                blt.w $C1CFBE
                cmpi.l #$C1ED48,(a1)
                bne.b .ready
                cmpi.l #$FFC00000,ALT_GUARD.l
                blt.w $C1CFBE
.ready:
                movem.w (a0)+,d2-d4
                move.l (a0),ALT_WORK.l
                move.w d7,d1
                lsr.w #8,d1
                move.w d1,ALT_KIND.l
                clr.b ALT_READY.l
