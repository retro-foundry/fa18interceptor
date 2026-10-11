; Byte-exact flagged-slot match evaluation $C26606-$C266AB.
; D0 is one when the first derived scalar is not greater than the second.

                org     $C26606

RETURN_NO_MATCH                equ     $C26602
C45785                         equ     $C45785
C45C32                         equ     $C45C32
C46184                         equ     $C46184
DERIVE_TRIPLE_SCALAR           equ     $C1D974

evaluate_c26606_flagged_slot:
                btst.b  #5,$26(a0)
                bne.s   .derive_scalars
                tst.b   C45785.l
                beq.s   RETURN_NO_MATCH
.derive_scalars:
                move.w  $2e(a0),d1
                asl.w   #8,d1
                add.w   d1,d1
                lea     C46184.l,a1
                adda.w  d1,a1
                lea     C45C32.l,a2
                movem.l $14(a1),d2-d4
                sub.l   $c(a2),d2
                bge.s   .first_x_nonnegative
                neg.l   d2
.first_x_nonnegative:
                sub.l   $10(a2),d3
                bge.s   .first_y_nonnegative
                neg.l   d3
.first_y_nonnegative:
                sub.l   $14(a2),d4
                bge.s   .first_z_nonnegative
                neg.l   d4
.first_z_nonnegative:
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                jsr     DERIVE_TRIPLE_SCALAR.l
                move.w  d1,-(sp)
                lea     C45C32.l,a2
                ; Preserve the observed zero-displacement address encoding.
                dc.w    $4ce8,$001c,$0000
                move.w  $30(a0),d5
                ext.l   d5
                swap    d5
                asl.l   #6,d5
                add.l   d5,d2
                move.w  $32(a0),d5
                ext.l   d5
                swap    d5
                asl.l   #6,d5
                add.l   d5,d4
                sub.l   $c(a2),d2
                bge.s   .second_x_nonnegative
                neg.l   d2
.second_x_nonnegative:
                sub.l   $10(a2),d3
                bge.s   .second_y_nonnegative
                neg.l   d3
.second_y_nonnegative:
                sub.l   $14(a2),d4
                bge.s   .second_z_nonnegative
                neg.l   d4
.second_z_nonnegative:
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                jsr     DERIVE_TRIPLE_SCALAR.l
                move.w  (sp)+,d2
                cmp.w   d1,d2
                bgt.w   RETURN_NO_MATCH
                moveq   #1,d0
                rts
