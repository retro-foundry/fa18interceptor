; Byte-exact observed candidate-result helper $C31C20-$C31C5D.
; Its callers use the signed condition code set by the final TST.W.

                org     $C31C20

ACTIVITY_FLAG                   equ     $C45837
CANDIDATE_FLAGS                 equ     $C458DB
CANDIDATE_ALT_FLAG              equ     $C457AE

return_c31c20_qualification_candidate:
                tst.b   ACTIVITY_FLAG.l
                bgt.b   .store_candidate
                move.w  (a1),d2
                blt.b   .clear_candidate_bit
                cmp.w   d2,d0
                beq.b   .return_rejected
                btst.b  #0,CANDIDATE_FLAGS.l
                bne.b   .return_rejected
                tst.b   CANDIDATE_ALT_FLAG.l
                bne.b   .clear_candidate_bit
                bra.b   .store_candidate
.return_rejected:
                moveq   #-1,d0
                rts
.clear_candidate_bit:
                andi.w  #$7FFF,(a1)
                andi.w  #$7FFF,d2
                move.w  d2,d0
                bra.b   .return_candidate
.store_candidate:
                move.w  d0,(a1)
                ori.w   #$8000,(a1)
.return_candidate:
                tst.w   d0
                rts
