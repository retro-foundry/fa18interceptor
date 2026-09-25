; Byte-exact observed value-adjustment and sign route $C31E28-$C31E35.

                org     $C31E28

prepare_c31e28_postflight_sign:
                subi.w  #$a,d0
                bge.b   $C31E36
                neg.w   d0
                move.b  #$20,(a2)
                bra.b   $C31E3A
