; Byte-exact observed C1EE14 stream continuation $C1EDCC-$C1EDD5.
; It loads the published stream cursor, consumes the next signed word, and
; takes the sentinel route for negative values.

                org     $C1EDCC

STREAM_STAGE_POINTER             equ     $C45A36

read_c1ee14_stream_next_word:
                movea.l STREAM_STAGE_POINTER.l,a2
                move.w  (a2)+,d0
                blt.b   $C1EDE8
