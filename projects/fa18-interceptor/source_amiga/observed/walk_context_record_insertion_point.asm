; Byte-exact observed context record insertion walk $C1E4D2-$C1E4E9.
; It begins at the preceding stream word, walks backwards while entries remain
; no smaller than D0, and routes to the record-output paths on exhaustion or
; the first smaller entry.

                org     $C1E4D2

walk_context_record_insertion_point:
                lea.l   -2(a0),a1
                subq.w  #1,d6
                bge.b   $C1E4E0
                move.w  $2C(a1),(a4)
                bra.b   $C1E4FE
                cmp.w   (a0)+,d0
                blt.b   $C1E4EA
                dbra    d6,$C1E4E0
                bra.b   $C1E4F4
