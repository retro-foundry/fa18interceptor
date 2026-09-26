; Byte-exact observed indexed-record transform continuation $C1E7E6-$C1E7F3.
; It follows two record-relative pointers, consumes the next signed word, and
; routes negative values to the alternate transform continuation.

                org     $C1E7E6

walk_indexed_record_transform_pointer:
                movea.l $2(a0),a4
                movea.l $10(a4),a4
                move.w  (a4)+,d0
                blt.w   $C1E872
