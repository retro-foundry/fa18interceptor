; Byte-exact observed activity-threshold continuation $C254A8-$C254B7.
; It divides the fixed numerator by D1 and compares the resulting word to the
; shared activity threshold before selecting the next route.

                org     $C254A8

ACTIVITY_SCALED_THRESHOLD      equ     $C45AE8

calculate_activity_scaled_threshold:
                move.l  #$c3500,d0
                divu.w  d1,d0
                cmp.w   ACTIVITY_SCALED_THRESHOLD.l,d0
                bge.b   $C254BE
