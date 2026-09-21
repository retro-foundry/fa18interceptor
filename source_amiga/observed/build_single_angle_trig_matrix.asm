; Byte-exact reconstruction of $C2E370-$C2E38D (Hunk 32 +$F68).
; Input: D4.w is a native angle word; A1 points to nine words.
; Output: writes a one-angle-derived 3x3 matrix in native trig scale.

                org     $C2E370

NATIVE_ANGLE_TO_TRIG_SHIFT equ 3
MATRIX_FIXED_AXIS_VALUE    equ $4000
lookup_sine_cosine         equ $C2E6DA

build_single_angle_trig_matrix:
                asr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d4
                bsr.w   lookup_sine_cosine
                move.w  d5,(a1)+
                clr.w   (a1)+
                move.w  d4,(a1)+
                clr.w   (a1)+
                move.w  #MATRIX_FIXED_AXIS_VALUE,(a1)+
                clr.w   (a1)+
                neg.w   d4
                move.w  d4,(a1)+
                clr.w   (a1)+
                move.w  d5,(a1)+
                rts
