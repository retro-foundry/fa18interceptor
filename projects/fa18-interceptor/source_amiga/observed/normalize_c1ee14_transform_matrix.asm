; Byte-exact transform-matrix normalization $C1F0A8-$C1F0ED.

                org     $C1F0A8

STREAM_RECORD_TABLE             equ $C46184
STREAM_RECORD_INDEX             equ $C459B6
STREAM_MATRIX_WORK              equ $C45BC6

normalize_c1ee14_transform_matrix:
                lea     STREAM_RECORD_TABLE.l,a0
                adda.w  STREAM_RECORD_INDEX.l,a0
                dc.w    $D0FC,$0092             ; adda.w  #$92,a0
                lea     STREAM_MATRIX_WORK.l,a2
                movem.w (a0)+,d0-d7
                asr.w   #6,d0
                asr.w   #6,d1
                asr.w   #6,d2
                asr.w   #6,d3
                asr.w   #6,d4
                asr.w   #6,d5
                asr.w   #6,d6
                asr.w   #6,d7
                movem.w d0-d7,(a2)
                move.w  (a0),d0
                asr.w   #6,d0
                move.w  d0,16(a2)
                lea     STREAM_RECORD_TABLE.l,a0
                adda.w  STREAM_RECORD_INDEX.l,a0
                dc.w    $D0FC,$00A4             ; adda.w  #$A4,a0
