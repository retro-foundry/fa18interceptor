; Byte-exact alternate transform-branch initialization $C1F464-$C1F4AB.
; The downstream branch at $C1F34A remains an external boundary.

                org     $C1F464

STREAM_ACCUMULATED_MATRIX       equ $C45BD8
STREAM_VECTOR_X                 equ $C45B30
STREAM_VECTOR_Y                 equ $C45B38
STREAM_VECTOR_Z                 equ $C45B34

initialize_c1ee14_alt_transform_branch:
                move.b  (a1)+,d3
                ext.w   d3
                addq.w  #1,a1
                lea     STREAM_ACCUMULATED_MATRIX.l,a2
                lea     (a2),a4
                movem.l -32(a6),d0-d2
                exg     d1,d2
                add.l   STREAM_VECTOR_X.l,d0
                add.l   STREAM_VECTOR_Y.l,d1
                add.l   STREAM_VECTOR_Z.l,d2
                moveq   #8,d7
                sub.w   -6(a6),d7
                asr.l   d7,d0
                asr.l   d7,d1
                asr.l   d7,d2
                movea.w d2,a5
                lea     -10(a6),a0
                subq.w  #1,d3
                move.w  d3,(a0)
                move.w  d6,d7
                andi.w  #2,d6
                bne.w   $C1F34A
