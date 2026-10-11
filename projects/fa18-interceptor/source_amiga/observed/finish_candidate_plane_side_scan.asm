; Byte-exact observed candidate plane-side scan continuation $C2741C-$C27447.
; A nonzero local count skips signed stream words before returning to the scan.
; Otherwise it tests selected-record bit 6 and joins the next candidate route.

                org     $C2741C

CANDIDATE_RECORD_BANK           equ     $C46184
CANDIDATE_RECORD_OFFSET         equ     $C459B6

finish_candidate_plane_side_scan:
                tst.w   -$30(a6)
                bne.w   $C271A8
skip_signed_plane_side_words:
                move.w  (a4)+,d0
                bge.b   skip_signed_plane_side_words
                bra.w   $C271A8
                tst.w   -$44(a6)
                bne.b   $C2744C
                lea     CANDIDATE_RECORD_BANK.l,a4
                move.w  CANDIDATE_RECORD_OFFSET.l,d0
                move.b  $4(a4,d0.w),d2
                andi.b  #$40,d2
                beq.b   $C2744C
