; Byte-exact observed postflight callback-code selector $C11002-$C11009.
; The callback byte at local -1 selects the following EF callback target.

                org     $C11002

gate_postflight_callback_code_ef:
                cmpi.b  #$EF,-1(a6)
                bne.b   $C11016
