; Byte-exact observed alternate C1F99A record-vector transform $C1FA92-$C1FAC9.
; It advances the descriptor-relative cursor, restores three local components,
; applies two shared vector offsets, shifts all three by the frame scale, and
; dispatches on bit 1 of d6.

                org     $C1FA92

STREAM_MATRIX_WORDS              equ     $C45BD8
STREAM_VECTOR_X                  equ     $C45B30
STREAM_VECTOR_Y                  equ     $C45B38

transform_c1f99a_alternate_record_vector:
                ; Preserve the binary's immediate ADDA encoding.
                dc.w    $D2FC,$000A             ; adda.w #$A,a1
                adda.w  d7,a1
                lea     STREAM_MATRIX_WORDS.l,a0
                lea     (a0),a4
                movem.l -$20(a6),d0-d2
                exg     d1,d2
                add.l   STREAM_VECTOR_X.l,d0
                add.l   STREAM_VECTOR_Y.l,d1
                moveq   #8,d3
                sub.w   -$6(a6),d3
                asr.l   d3,d0
                asr.l   d3,d1
                asr.l   d3,d2
                move.w  d2,-$12(a6)
                andi.w  #2,d6
                bne.b   $C1FB24
