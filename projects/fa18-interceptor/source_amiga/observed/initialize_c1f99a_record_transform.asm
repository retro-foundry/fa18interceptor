; Byte-exact positive-record transform setup $C1F99A-$C1F9ED.
; Record and matrix ownership remain structural.

                org     $C1F99A

OFFSET_VERTEX_TABLE             equ $C48390
RECORD_TRANSFORM_DESCRIPTOR     equ $C45A32
STREAM_MATRIX_WORK              equ $C45BC6
STREAM_VECTOR_X                 equ $C45B30

initialize_c1f99a_record_transform:
                lea     OFFSET_VERTEX_TABLE.l,a3
                move.l  a1,-(sp)
                move.w  d0,-10(a6)
                movea.l RECORD_TRANSFORM_DESCRIPTOR.l,a1
                move.b  7(a1),d6
                btst    #0,d6
                beq.w   $C1FA92
                movem.l a5/a2,-(sp)
                lea     10(a1,d7.w),a1
                lea     STREAM_MATRIX_WORK.l,a2
                lea     (a2),a4
                adda.w  d7,a3
                movem.l -32(a6),d0-d2
                exg     d1,d2
                movem.l STREAM_VECTOR_X.l,d3-d5
                add.l   d3,d0
                add.l   d4,d2
                add.l   d5,d1
                moveq   #8,d7
                sub.w   -6(a6),d7
                asr.l   d7,d0
                asr.l   d7,d1
                asr.l   d7,d2
                movea.w d2,a0
