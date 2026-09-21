; Byte-exact reconstruction of $C2E346-$C2E36F (Hunk 32 +$F3E).
; Input: D4.w is a native angle word; A1 points to nine words.
; Effect: writes a one-angle-derived 3x3 fixed-point matrix.

                org     $C2E346

NATIVE_ANGLE_TO_TRIG_SHIFT equ 3
TRIG_TO_MATRIX_SHIFT       equ 6
MATRIX_FIXED_AXIS_VALUE    equ $0100
lookup_sine_cosine         equ $C2E6DA

build_single_angle_matrix:
                asr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d4
                bsr.w   lookup_sine_cosine
                move.w  d5,d6
                asr.w   #TRIG_TO_MATRIX_SHIFT,d6
                move.w  d6,(a1)+
                clr.w   (a1)+
                move.w  d4,d6
                asr.w   #TRIG_TO_MATRIX_SHIFT,d6
                move.w  d6,(a1)+
                clr.w   (a1)+
                move.w  #MATRIX_FIXED_AXIS_VALUE,(a1)+
                clr.w   (a1)+
                asr.w   #TRIG_TO_MATRIX_SHIFT,d4
                neg.w   d4
                move.w  d4,(a1)+
                clr.w   (a1)+
                asr.w   #TRIG_TO_MATRIX_SHIFT,d5
                move.w  d5,(a1)+
                rts
