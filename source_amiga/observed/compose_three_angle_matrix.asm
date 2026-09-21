; Byte-exact reconstruction of $C2E3DE-$C2E479 (Hunk 32 +$BD6).
; Input: D0.w, D2.w, D4.w are native angle words; A1 points to nine words.
; Output: nine signed matrix words at A1..A1+$10.
; Clobbers: D0-D7, A0. A1 advances by 16 bytes.
;
; This is a sibling of build_rotation_matrix.  Its mixed MULS/SWAP scaling
; sequence is retained exactly; it has not yet been algebraically normalized.

                org     $C2E3DE

NATIVE_ANGLE_TO_TRIG_SHIFT equ 3
TRIG_PRODUCT_SHIFT         equ 14
POST_SWAP_PRODUCT_SHIFT    equ 4
lookup_two_sine_cosine     equ $C2E5F6
lookup_sine_cosine         equ $C2E6DA

compose_three_angle_matrix:
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d0
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d2
                lsr.w   #NATIVE_ANGLE_TO_TRIG_SHIFT,d4
                bsr.w   lookup_two_sine_cosine
                bsr.w   lookup_sine_cosine

                move.w  d2,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d4,d6
                move.w  d3,d7
                muls.w  d5,d7
                add.l   d6,d7
                swap    d7
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d7
                move.w  d7,(a1)+

                move.w  d1,d6
                muls.w  d4,d6
                swap    d6
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d6
                neg.w   d6
                move.w  d6,(a1)+

                move.w  d3,d6
                muls.w  d0,d6
                swap    d7
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d4,d6
                move.w  d2,d7
                muls.w  d5,d7
                sub.l   d6,d7
                swap    d7
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d7
                move.w  d7,(a1)+

                move.w  d2,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d5,d6
                move.w  d3,d7
                muls.w  d4,d7
                sub.l   d6,d7
                swap    d7
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d7
                move.w  d7,(a1)+

                move.w  d1,d6
                muls.w  d5,d6
                swap    d6
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d6
                move.w  d6,(a1)+

                move.w  d3,d6
                muls.w  d0,d6
                moveq   #TRIG_PRODUCT_SHIFT,d7
                asr.l   d7,d6
                muls.w  d5,d6
                move.w  d2,d7
                muls.w  d4,d7
                add.l   d6,d7
                swap    d7
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d7
                move.w  d7,(a1)+

                move.w  d2,d6
                muls.w  d1,d6
                swap    d6
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d6
                neg.w   d6
                move.w  d6,(a1)+

                asr.w   #6,d0
                neg.w   d0
                move.w  d0,(a1)+

                move.w  d3,d6
                muls.w  d1,d6
                swap    d6
                asr.w   #POST_SWAP_PRODUCT_SHIFT,d6
                move.w  d6,(a1)
                rts
