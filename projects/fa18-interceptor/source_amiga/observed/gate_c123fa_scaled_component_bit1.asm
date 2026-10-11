; Byte-exact observed C123FA scaled-component bit-1 gate $C126D2-$C126D9.
; A set bit skips the complementary local -$0E adjustment.

                org     $C126D2

gate_c123fa_scaled_component_bit1:
                btst.b  #1,-$9(a6)
                bne.b   $C126E8
