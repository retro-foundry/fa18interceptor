; Byte-exact observed matrix-update tail $C2D94E-$C2D99B.

                org     $C2D94E

CALL_BUILD_MATRIX_FROM_TRIPLE   equ     $C2E47A
CALL_COMPOSE_MATRIX_FROM_TRIPLE equ     $C2E514

publish_record_matrix_update_triple:
                dc.w    $08A9,$0002,$0003 ; bclr.b #2,3(a1); retain original EA
                movem.w d4-d6,$66(a1)
                dc.w    $48E7,$4E40 ; movem.l d1/d4-d6/a1,-(a7)
                move.w  d4,d0
                move.w  d5,d2
                move.w  d6,d4
                lea.l   $80(a1),a1
                bsr.w   CALL_BUILD_MATRIX_FROM_TRIPLE
                dc.w    $4CDF,$02E2 ; movem.l (a7)+,d1/d5-d7/a1
                dc.w    $D2FC,$0092 ; adda.w #$92,a1; retain original opcode
                moveq   #0,d0
                moveq   #0,d2
                moveq   #0,d4
                move.w  #$7080,d1
                tst.w   d5
                beq.b   .d5_zero
                move.w  d1,d0
                sub.w   d5,d0
.d5_zero:
                tst.w   d6
                beq.b   .d6_zero
                move.w  d1,d2
                sub.w   d6,d2
.d6_zero:
                tst.w   d7
                beq.b   .d7_zero
                move.w  d1,d4
                sub.w   d7,d4
.d7_zero:
                bsr.w   CALL_COMPOSE_MATRIX_FROM_TRIPLE
                rts
