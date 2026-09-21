; Byte-exact reconstruction of $C2E514-$C2E5AB (Hunk 32 +$70C).
; Input: D0.w, D2.w, D4.w are native angle words; A1 points to nine words.
; Output: nine signed matrix words at A1..A1+$10.

                org     $C2E514

NATIVE_ANGLE_TO_TRIG_SHIFT equ 3
TRIG_PRODUCT_SHIFT         equ 14
lookup_two_sine_cosine     equ $C2E5F6
lookup_sine_cosine         equ $C2E6DA

compose_alternate_three_angle_matrix:
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d0
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d2
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d4
                bsr.w   lookup_two_sine_cosine
                bsr.w   lookup_sine_cosine
                move.w  d4,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d2,d6
                move.w  d5,d7
                muls.w  d3,d7
                sub.l   d6,d7
                moveq   #TRIG_PRODUCT_SHIFT,d6
                asr.l   d6,d7
                move.w  d7,(a1)+
                move.w  d5,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d2,d6
                move.w  d4,d7
                muls.w  d3,d7
                add.l   d6,d7
                moveq   #TRIG_PRODUCT_SHIFT,d6
                asr.l   d6,d7
                neg.w   d7
                move.w  d7,(a1)+
                move.w  d1,d6
                muls.w  d2,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                move.w  d6,(a1)+
                move.w  d4,d6
                muls.w  d1,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                move.w  d6,(a1)+
                move.w  d5,d6
                muls.w  d1,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                move.w  d6,(a1)+
                move.w  d0,d6
                move.w  d6,(a1)+
                move.w  d4,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d3,d6
                move.w  d5,d7
                muls.w  d2,d7
                add.l   d6,d7
                moveq   #TRIG_PRODUCT_SHIFT,d6
                asr.l   d6,d7
                neg.w   d7
                move.w  d7,(a1)+
                move.w  d5,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d3,d6
                move.w  d4,d7
                muls.w  d2,d7
                sub.l   d6,d7
                moveq   #TRIG_PRODUCT_SHIFT,d6
                asr.l   d6,d7
                move.w  d7,(a1)+
                move.w  d1,d6
                muls.w  d3,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                move.w  d6,(a1)
                rts
