; Byte-exact observed candidate-pair constraint path $C2718C-$C2721F.
; The first entry restores saved components and rejoins the scan. The sibling
; entry derives signed pair differences and gates their two-product sum.

                org     $C2718C

RESUME_CANDIDATE_SCAN           equ     $C26F00
CANDIDATE_REJECT_ROUTE          equ     $C2742C
CANDIDATE_ACCEPT_ROUTE          equ     $C2741C
CANDIDATE_SPECIAL_ROUTE         equ     $C27234

evaluate_candidate_pair_constraint:
                movem.l -$2e(a6),d2-d4
                move.w  (sp)+,d0
                bra.w   RESUME_CANDIDATE_SCAN

.candidate_pair_entry:
                move.w  2(a3,d7.w),d1
                move.w  -$5a(a6),d0
                asr.w   d0,d1
                move.w  d1,-$34(a6)
                lea     (a5),a4
                clr.w   -$30(a6)
.read_candidate_header:
                move.w  (a4),-$32(a6)
                blt.w   CANDIDATE_REJECT_ROUTE
.candidate_item:
                move.w  (a4)+,d0
                bge.s   .next_header_nonnegative
                andi.w  #$fff,d0
                move.w  -$32(a6),d1
                addq.w  #1,-$30(a6)
                bra.s   .header_ready
.next_header_nonnegative:
                move.w  (a4),d1
                andi.w  #$fff,d1
.header_ready:
                addi.w  #$a4,d0
                addi.w  #$a4,d1
                lea     (a3,d0.w),a5
                movem.w (a3,d1.w),d5-d7
                sub.w   (a5),d5
                sub.w   4(a5),d7
                neg.w   d7
                exg     d5,d7
                move.w  (a5),d2
                move.w  4(a5),d4
                move.w  -$5a(a6),d0
                asr.w   d0,d2
                asr.w   d0,d4
                add.w   $c(a3),d2
                add.w   $e(a3),d4
                sub.w   a2,d2
                sub.w   -$42(a6),d4
                neg.w   d2
                neg.w   d4
                muls.w  d2,d5
                muls.w  d4,d7
                add.l   d5,d7
                bge.w   CANDIDATE_ACCEPT_ROUTE
                tst.w   -$30(a6)
                beq.s   .candidate_item
                cmpi.b  #$20,$62(a3)
                beq.s   CANDIDATE_SPECIAL_ROUTE
