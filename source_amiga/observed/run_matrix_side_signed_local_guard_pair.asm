; Byte-exact observed matrix-side signed-local guard pair $C136A0-$C136BD.
; It invokes the two current-record word guards with the same sign-extended
; local word, then rejoins the shared matrix-side continuation.

                org     $C136A0

CURRENT_RECORD_FIRST_WORD_GUARD equ     $C13A8E
CURRENT_RECORD_SECOND_WORD_GUARD equ     $C13B5A

run_matrix_side_signed_local_guard_pair:
                move.w  -2(a6),d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   CURRENT_RECORD_FIRST_WORD_GUARD
                addq.l  #4,a7
                move.w  -2(a6),d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   CURRENT_RECORD_SECOND_WORD_GUARD
                addq.l  #4,a7
                bra.b   $C1371E
