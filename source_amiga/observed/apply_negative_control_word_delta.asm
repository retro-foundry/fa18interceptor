; Byte-exact observed signed-negative half of helper $C13CDE, $C13D14-$C13D33.
; It suppresses the local delta below the negative bound, otherwise subtracts
; it from the caller-supplied word pointer and returns.

                org     $C13D14

apply_negative_control_word_delta:
                movea.l 8(a6),a0
                move.w  (a0),d0
                cmpi.w  #$FB00,d0
                ble.b   .apply_delta
                clr.w   -$2(a6)
.apply_delta:
                movea.l 8(a6),a0
                move.w  (a0),d0
                sub.w   -$2(a6),d0
                move.w  d0,(a0)
                unlk    a6
                rts
