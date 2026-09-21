; Byte-exact reconstruction of $C2E38E-$C2E3DD (Hunk 32 +$F86).
; Input: D0.w and D2.w are native angle words; A1 points to nine words.
; Effect: writes a two-angle-derived 3x3 fixed-point matrix.

                org     $C2E38E

NATIVE_ANGLE_TO_TRIG_SHIFT equ 3
TRIG_TO_MATRIX_SHIFT       equ 6
PRODUCT_HIGHWORD_SHIFT     equ 4
lookup_two_sine_cosine     equ $C2E5F6

build_two_angle_matrix:
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d0
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d2
                bsr.w   lookup_two_sine_cosine

                move.w  d3,d6
                asr.w   #TRIG_TO_MATRIX_SHIFT,d6
                move.w  d6,(a1)+
                clr.w   (a1)+
                move.w  d2,d6
                asr.w   #TRIG_TO_MATRIX_SHIFT,d6
                move.w  d6,(a1)+

                move.w  d2,d6
                muls.w  d0,d6
                swap    d6
                asr.w   #PRODUCT_HIGHWORD_SHIFT,d6
                neg.w   d6
                move.w  d6,(a1)+
                move.w  d1,d6
                asr.w   #TRIG_TO_MATRIX_SHIFT,d6
                move.w  d6,(a1)+

                move.w  d3,d6
                muls.w  d0,d6
                swap    d6
                asr.w   #PRODUCT_HIGHWORD_SHIFT,d6
                move.w  d6,(a1)+
                move.w  d2,d6
                muls.w  d1,d6
                swap    d6
                asr.w   #PRODUCT_HIGHWORD_SHIFT,d6
                neg.w   d6
                move.w  d6,(a1)+

                neg.w   d0
                asr.w   #TRIG_TO_MATRIX_SHIFT,d0
                move.w  d0,(a1)+
                move.w  d3,d6
                muls.w  d1,d6
                swap    d6
                asr.w   #PRODUCT_HIGHWORD_SHIFT,d6
                move.w  d6,(a1)
                rts
