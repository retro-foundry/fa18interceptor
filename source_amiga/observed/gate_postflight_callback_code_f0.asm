; Byte-exact observed postflight callback-code selector $C10FEE-$C10FF5.
; The callback byte at local -1 selects the following F0 callback target.

                org     $C10FEE

gate_postflight_callback_code_f0:
                cmpi.b  #$F0,-1(a6)
                bne.b   $C11002
