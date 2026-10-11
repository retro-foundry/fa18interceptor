; Byte-exact observed matrix-side first-word guard invocation $C136D6-$C136E5.
; It sign-extends the local word, passes it to the current-record word guard,
; then rejoins the shared matrix-side continuation.

                org     $C136D6

CURRENT_RECORD_FIRST_WORD_GUARD equ     $C13A8E

run_matrix_side_first_word_guard:
                move.w  -$2(a6),d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   CURRENT_RECORD_FIRST_WORD_GUARD
                addq.l  #4,a7
                bra.b   $C1371E
